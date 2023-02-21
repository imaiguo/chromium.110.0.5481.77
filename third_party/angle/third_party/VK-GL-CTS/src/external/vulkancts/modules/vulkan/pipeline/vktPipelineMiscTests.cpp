/*------------------------------------------------------------------------
 * Vulkan Conformance Tests
 * ------------------------
 *
 * Copyright (c) 2019 Google LLC
 * Copyright (c) 2019 The Khronos Group Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *//*!
 * \file
 * \brief Miscellaneous pipeline tests.
 *//*--------------------------------------------------------------------*/

#include <string>

#include "vktTestGroupUtil.hpp"
#include "vktAmberTestCase.hpp"
#include "vktPipelineMiscTests.hpp"

#include "tcuImageCompare.hpp"
#include "vkImageUtil.hpp"
#include "deStringUtil.hpp"
#include "vktTestCaseUtil.hpp"
#include "vkTypeUtil.hpp"
#include "vkCmdUtil.hpp"
#include "vkObjUtil.hpp"
#include "vkImageWithMemory.hpp"
#include "vkBarrierUtil.hpp"
#include "vkBufferWithMemory.hpp"
#include "vktPipelineReferenceRenderer.hpp"

namespace vkt
{
namespace pipeline
{

using namespace vk;

namespace
{

enum AmberFeatureBits
{
	AMBER_FEATURE_VERTEX_PIPELINE_STORES_AND_ATOMICS	= (1 <<	0),
	AMBER_FEATURE_TESSELATION_SHADER					= (1 <<	1),
	AMBER_FEATURE_GEOMETRY_SHADER						= (1 <<	2),
};

using AmberFeatureFlags = deUint32;

#ifndef CTS_USES_VULKANSC
std::vector<std::string> getFeatureList (AmberFeatureFlags flags)
{
	std::vector<std::string> requirements;

	if (flags & AMBER_FEATURE_VERTEX_PIPELINE_STORES_AND_ATOMICS)
		requirements.push_back("Features.vertexPipelineStoresAndAtomics");

	if (flags & AMBER_FEATURE_TESSELATION_SHADER)
		requirements.push_back("Features.tessellationShader");

	if (flags & AMBER_FEATURE_GEOMETRY_SHADER)
		requirements.push_back("Features.geometryShader");

	return requirements;
}
#endif // CTS_USES_VULKANSC

void addMonolithicAmberTests (tcu::TestCaseGroup* tests)
{
#ifndef CTS_USES_VULKANSC
	tcu::TestContext& testCtx = tests->getTestContext();

	// Shader test files are saved in <path>/external/vulkancts/data/vulkan/amber/pipeline/<basename>.amber
	struct Case {
		const char*			basename;
		const char*			description;
		AmberFeatureFlags	flags;
	};

	const Case cases[] =
	{
		{
			"position_to_ssbo",
			"Write position data into ssbo using only the vertex shader in a pipeline",
			(AMBER_FEATURE_VERTEX_PIPELINE_STORES_AND_ATOMICS),
		},
		{
			"primitive_id_from_tess",
			"Read primitive id from tessellation shaders without a geometry shader",
			(AMBER_FEATURE_TESSELATION_SHADER | AMBER_FEATURE_GEOMETRY_SHADER),
		},
	};
	for (unsigned i = 0; i < DE_LENGTH_OF_ARRAY(cases) ; ++i)
	{
		std::string					file			= std::string(cases[i].basename) + ".amber";
		std::vector<std::string>	requirements	= getFeatureList(cases[i].flags);
		cts_amber::AmberTestCase	*testCase		= cts_amber::createAmberTestCase(testCtx, cases[i].basename, cases[i].description, "pipeline", file, requirements);

		tests->addChild(testCase);
	}
#else
	DE_UNREF(tests);
#endif
}

class ImplicitPrimitiveIDPassthroughCase : public vkt::TestCase
{
public:
	ImplicitPrimitiveIDPassthroughCase		(tcu::TestContext&                  testCtx,
											 const std::string&                 name,
											 const std::string&                 description,
											 const PipelineConstructionType		pipelineConstructionType,
											 bool withTessellation)
		: vkt::TestCase(testCtx, name, description)
		, m_pipelineConstructionType(pipelineConstructionType)
		, m_withTessellationPassthrough(withTessellation)
	{
	}
	~ImplicitPrimitiveIDPassthroughCase		    (void) {}
	void			initPrograms				(SourceCollections& programCollection) const override;
	void			checkSupport				(Context& context) const override;
	TestInstance*	createInstance				(Context& context) const override;

	const PipelineConstructionType m_pipelineConstructionType;
private:
	bool m_withTessellationPassthrough;
};

class ImplicitPrimitiveIDPassthroughInstance : public vkt::TestInstance
{
public:
	ImplicitPrimitiveIDPassthroughInstance	(Context&                           context,
											 const PipelineConstructionType		pipelineConstructionType,
											 bool withTessellation)
		:
		vkt::TestInstance(context)
		, m_renderSize		        (2, 2)
		, m_extent(makeExtent3D(m_renderSize.x(), m_renderSize.y(), 1u))
		, m_graphicsPipeline		(context.getDeviceInterface(), context.getDevice(), pipelineConstructionType)
		, m_withTessellationPassthrough(withTessellation)
	{
	}
	~ImplicitPrimitiveIDPassthroughInstance	(void) {}
	tcu::TestStatus		iterate				(void) override;

private:
	const tcu::UVec2            m_renderSize;
	const VkExtent3D		    m_extent;
	const VkFormat		        m_format = VK_FORMAT_R8G8B8A8_UNORM;
	GraphicsPipelineWrapper		m_graphicsPipeline;
	bool                        m_withTessellationPassthrough;
};

TestInstance* ImplicitPrimitiveIDPassthroughCase::createInstance (Context& context) const
{
	return new ImplicitPrimitiveIDPassthroughInstance(context, m_pipelineConstructionType, m_withTessellationPassthrough);
}

void ImplicitPrimitiveIDPassthroughCase::checkSupport (Context &context) const
{
	if (m_withTessellationPassthrough)
		context.requireDeviceCoreFeature(DEVICE_CORE_FEATURE_TESSELLATION_SHADER);

	checkPipelineLibraryRequirements(context.getInstanceInterface(), context.getPhysicalDevice(), m_pipelineConstructionType);
}

void ImplicitPrimitiveIDPassthroughCase::initPrograms(SourceCollections& sources) const
{
	std::ostringstream vert;
	// Generate a vertically split framebuffer, filled with red on the
	// left, and a green on the right.
	vert
		<< glu::getGLSLVersionDeclaration(glu::GLSL_VERSION_450) << "\n"
		<< "void main ()\n"
		<< "{\n"
		<< "    switch (gl_VertexIndex) {\n"
		<< "        case 0:\n"
		<< "            gl_Position = vec4(-3.0, -1.0, 0.0, 1.0);\n"
		<< "            break;\n"
		<< "        case 1:\n"
		<< "            gl_Position = vec4(0.0, 3.0, 0.0, 1.0);\n"
		<< "            break;\n"
		<< "        case 2:\n"
		<< "            gl_Position = vec4(0.0, -1.0, 0.0, 1.0);\n"
		<< "            break;\n"
		<< "        case 3:\n"
		<< "            gl_Position = vec4(0.0, -1.0, 0.0, 1.0);\n"
		<< "            break;\n"
		<< "        case 4:\n"
		<< "            gl_Position = vec4(3.0, -1.0, 0.0, 1.0);\n"
		<< "            break;\n"
		<< "        case 5:\n"
		<< "            gl_Position = vec4(0.0, 3.0, 0.0, 1.0);\n"
		<< "            break;\n"
		<< "    }\n"
		<< "}\n"
		;
	sources.glslSources.add("vert") << glu::VertexSource(vert.str());

	if (m_withTessellationPassthrough) {
		std::ostringstream tsc;
		tsc
			<< glu::getGLSLVersionDeclaration(glu::GLSL_VERSION_450) << "\n"
			<< "layout (vertices = 3) out;\n"
			<< "\n"
			<< "void main ()\n"
			<< "{\n"
			<< "    if (gl_InvocationID == 0) {\n"
			<< "        gl_TessLevelInner[0] = 1.0;\n"
			<< "        gl_TessLevelInner[1] = 1.0;\n"
			<< "        gl_TessLevelOuter[0] = 1.0;\n"
			<< "        gl_TessLevelOuter[1] = 1.0;\n"
			<< "        gl_TessLevelOuter[2] = 1.0;\n"
			<< "        gl_TessLevelOuter[3] = 1.0;\n"
			<< "    }\n"
			<< "    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;\n"
			<< "}\n";
		sources.glslSources.add("tsc") << glu::TessellationControlSource(tsc.str());

		std::ostringstream tse;
		tse
			<< glu::getGLSLVersionDeclaration(glu::GLSL_VERSION_450) << "\n"
			<< "layout (triangles, equal_spacing, cw) in;\n"
			<< "\n"
			<< "void main ()\n"
			<< "{\n"
			<< "    gl_Position = gl_in[0].gl_Position * gl_TessCoord.x +\n"
			<< "                  gl_in[1].gl_Position * gl_TessCoord.y +\n"
			<< "                  gl_in[2].gl_Position * gl_TessCoord.z;\n"
			<< "}\n"
			;
		sources.glslSources.add("tse") << glu::TessellationEvaluationSource(tse.str());
	}

	std::ostringstream frag;
	frag
		<< glu::getGLSLVersionDeclaration(glu::GLSL_VERSION_450) << "\n"
		<< "layout (location=0) out vec4 outColor;\n"
		<< "\n"
		<< "void main ()\n"
		<< "{\n"
		<< "    const vec4 red = vec4(1.0, 0.0, 0.0, 1.0);\n"
		<< "    const vec4 green = vec4(0.0, 1.0, 0.0, 1.0);\n"
		<< "    outColor = (gl_PrimitiveID % 2 == 0) ? red : green;\n"
		<< "}\n"
		;
	sources.glslSources.add("frag") << glu::FragmentSource(frag.str());
}

tcu::TestStatus ImplicitPrimitiveIDPassthroughInstance::iterate ()
{
	const auto&			vkd					= m_context.getDeviceInterface();
	const auto			device				= m_context.getDevice();
	auto&				alloc				= m_context.getDefaultAllocator();
	const auto			qIndex				= m_context.getUniversalQueueFamilyIndex();
	const auto			queue				= m_context.getUniversalQueue();
	const auto			tcuFormat			= mapVkFormat(m_format);
	const auto			colorUsage			= (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
	const auto			verifBufferUsage	= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	const tcu::Vec4		clearColor			(0.0f, 0.0f, 0.0f, 1.0f);

	// Color attachment.
	const VkImageCreateInfo colorBufferInfo =
	{
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,	//	VkStructureType			sType;
		nullptr,								//	const void*				pNext;
		0u,										//	VkImageCreateFlags		flags;
		VK_IMAGE_TYPE_2D,						//	VkImageType				imageType;
		m_format,								//	VkFormat				format;
		m_extent,								//	VkExtent3D				extent;
		1u,										//	uint32_t				mipLevels;
		1u,										//	uint32_t				arrayLayers;
		VK_SAMPLE_COUNT_1_BIT,					//	VkSampleCountFlagBits	samples;
		VK_IMAGE_TILING_OPTIMAL,				//	VkImageTiling			tiling;
		colorUsage,								//	VkImageUsageFlags		usage;
		VK_SHARING_MODE_EXCLUSIVE,				//	VkSharingMode			sharingMode;
		0u,										//	uint32_t				queueFamilyIndexCount;
		nullptr,								//	const uint32_t*			pQueueFamilyIndices;
		VK_IMAGE_LAYOUT_UNDEFINED,				//	VkImageLayout			initialLayout;
	};
	ImageWithMemory		colorBuffer		(vkd, device, alloc, colorBufferInfo, MemoryRequirement::Any);
	const auto			colorSRR		= makeImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u);
	const auto			colorSRL		= makeImageSubresourceLayers(VK_IMAGE_ASPECT_COLOR_BIT, 0u, 0u, 1u);
	const auto			colorBufferView	= makeImageView(vkd, device, colorBuffer.get(), VK_IMAGE_VIEW_TYPE_2D, m_format, colorSRR);

	// Verification buffer.
	const auto			verifBufferSize		= static_cast<VkDeviceSize>(tcu::getPixelSize(tcuFormat)) * m_extent.width * m_extent.height;
	const auto			verifBufferInfo		= makeBufferCreateInfo(verifBufferSize, verifBufferUsage);
	BufferWithMemory	verifBuffer			(vkd, device, alloc, verifBufferInfo, MemoryRequirement::HostVisible);
	auto&				verifBufferAlloc	= verifBuffer.getAllocation();

	// Render pass and framebuffer.
	const auto renderPass	= makeRenderPass(vkd, device, m_format);
	const auto framebuffer	= makeFramebuffer(vkd, device, renderPass.get(), colorBufferView.get(), m_extent.width, m_extent.height);

	// Shader modules.
	const auto&		binaries		= m_context.getBinaryCollection();
	const auto		vertModule		= createShaderModule(vkd, device, binaries.get("vert"));
	const auto		fragModule		= createShaderModule(vkd, device, binaries.get("frag"));
	Move<VkShaderModule> tscModule;
	Move<VkShaderModule> tseModule;

	if (m_withTessellationPassthrough) {
		tscModule = createShaderModule(vkd, device, binaries.get("tsc"));
		tseModule = createShaderModule(vkd, device, binaries.get("tse"));
	}

	// Viewports and scissors.
	const std::vector<VkViewport>	viewports	(1u, makeViewport(m_extent));
	const std::vector<VkRect2D>		scissors	(1u, makeRect2D(m_extent));

	const VkPipelineVertexInputStateCreateInfo		vertexInputState	= initVulkanStructure();
	const VkPipelineRasterizationStateCreateInfo    rasterizationState  =
	{
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,		// VkStructureType                          sType;
		DE_NULL,														// const void*                              pNext;
		(VkPipelineRasterizationStateCreateFlags)0,						// VkPipelineRasterizationStateCreateFlags  flags;
		VK_FALSE,														// VkBool32                                 depthClampEnable;
		VK_FALSE,														// VkBool32                                 rasterizerDiscardEnable;
		VK_POLYGON_MODE_FILL,											// VkPolygonMode							polygonMode;
		VK_CULL_MODE_NONE,												// VkCullModeFlags							cullMode;
		VK_FRONT_FACE_CLOCKWISE,								// VkFrontFace								frontFace;
		VK_FALSE,														// VkBool32									depthBiasEnable;
		0.0f,															// float									depthBiasConstantFactor;
		0.0f,															// float									depthBiasClamp;
		0.0f,															// float									depthBiasSlopeFactor;
		1.0f,															// float									lineWidth;
	};

	// Pipeline layout and graphics pipeline.
	const auto pipelineLayout	= makePipelineLayout(vkd, device);

	const auto topology = m_withTessellationPassthrough ? VK_PRIMITIVE_TOPOLOGY_PATCH_LIST : VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	m_graphicsPipeline.setDefaultRasterizationState()
		.setDefaultTopology(topology)
		.setupVertexInputState(&vertexInputState)
		.setDefaultDepthStencilState()
		.setDefaultMultisampleState()
		.setDefaultColorBlendState()
		.setupPreRasterizationShaderState(viewports, scissors, *pipelineLayout, *renderPass, 0u, *vertModule, &rasterizationState, *tscModule, *tseModule)
		.setupFragmentShaderState(*pipelineLayout, *renderPass, 0u, *fragModule)
		.setupFragmentOutputState(*renderPass)
		.setMonolithicPipelineLayout(*pipelineLayout)
		.buildPipeline();

	// Command pool and buffer.
	const auto cmdPool		= makeCommandPool(vkd, device, qIndex);
	const auto cmdBufferPtr	= allocateCommandBuffer(vkd, device, cmdPool.get(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	const auto cmdBuffer	= cmdBufferPtr.get();

	beginCommandBuffer(vkd, cmdBuffer);

	// Draw.
	beginRenderPass(vkd, cmdBuffer, renderPass.get(), framebuffer.get(), scissors.at(0u), clearColor);
	vkd.cmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline.getPipeline());
	vkd.cmdDraw(cmdBuffer, 6, 1u, 0u, 0u);
	endRenderPass(vkd, cmdBuffer);

	// Copy to verification buffer.
	const auto copyRegion		= makeBufferImageCopy(m_extent, colorSRL);
	const auto transfer2Host	= makeMemoryBarrier(VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_HOST_READ_BIT);
	const auto color2Transfer	= makeImageMemoryBarrier(
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		colorBuffer.get(), colorSRR);

	cmdPipelineImageMemoryBarrier(vkd, cmdBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, &color2Transfer);
	vkd.cmdCopyImageToBuffer(cmdBuffer, colorBuffer.get(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, verifBuffer.get(), 1u, &copyRegion);
	cmdPipelineMemoryBarrier(vkd, cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT, &transfer2Host);

	endCommandBuffer(vkd, cmdBuffer);

	// Submit and validate result.
	submitCommandsAndWait(vkd, device, queue, cmdBuffer);

	auto& log = m_context.getTestContext().getLog();
	const tcu::IVec3					iExtent (static_cast<int>(m_extent.width), static_cast<int>(m_extent.height), static_cast<int>(m_extent.depth));
	void*								verifBufferData		= verifBufferAlloc.getHostPtr();
	const tcu::ConstPixelBufferAccess	verifAccess		(tcuFormat, iExtent, verifBufferData);
	invalidateAlloc(vkd, device, verifBufferAlloc);

	const auto red = tcu::Vec4(1.0f, 0.0f, 0.0f, 1.0f);
	const auto green = tcu::Vec4(0.0f, 1.0f, 0.0f, 1.0f);

	for (int x = 0; x < iExtent.x(); ++x)
		for (int y = 0; y < iExtent.y(); ++y) {
			const auto resultColor = verifAccess.getPixel(x, y);
			const auto expectedColor = (x < iExtent.x() / 2) ? red : green;
			if (resultColor != expectedColor) {
				log << tcu::TestLog::ImageSet("Result image", "Expect left side of framebuffer red, and right side green")
					<< tcu::TestLog::Image("Result", "Verification buffer", verifAccess)
					<< tcu::TestLog::EndImageSet;
				TCU_FAIL("Expected a vertically split framebuffer, filled with red on the left and green the right; see the log for the unexpected result");
			}
		}

	return tcu::TestStatus::pass("Pass");
}

} // anonymous


void createMiscTests (tcu::TestCaseGroup* miscTests, PipelineConstructionType pipelineConstructionType)
{
	// Location of the Amber script files under the data/vulkan/amber source tree.
	if (pipelineConstructionType == PIPELINE_CONSTRUCTION_TYPE_MONOLITHIC) {
		addMonolithicAmberTests(miscTests);
	}

	tcu::TestContext& testCtx = miscTests->getTestContext();
	miscTests->addChild(new ImplicitPrimitiveIDPassthroughCase(testCtx, "implicit_primitive_id", "Verify implicit access to gl_PrimtiveID works", pipelineConstructionType, false));
	miscTests->addChild(new ImplicitPrimitiveIDPassthroughCase(testCtx, "implicit_primitive_id_with_tessellation", "Verify implicit access to gl_PrimtiveID works with a tessellation shader", pipelineConstructionType, true));

}

} // pipeline
} // vkt
