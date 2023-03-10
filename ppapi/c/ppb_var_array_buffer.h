/* Copyright 2012 The Chromium Authors
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* From ppb_var_array_buffer.idl modified Thu Feb 28 09:24:06 2013. */

#ifndef PPAPI_C_PPB_VAR_ARRAY_BUFFER_H_
#define PPAPI_C_PPB_VAR_ARRAY_BUFFER_H_

#include "ppapi/c/pp_bool.h"
#include "ppapi/c/pp_macros.h"
#include "ppapi/c/pp_stdint.h"
#include "ppapi/c/pp_var.h"

#define PPB_VAR_ARRAY_BUFFER_INTERFACE_1_0 "PPB_VarArrayBuffer;1.0"
#define PPB_VAR_ARRAY_BUFFER_INTERFACE PPB_VAR_ARRAY_BUFFER_INTERFACE_1_0

/**
 * @file
 * This file defines the <code>PPB_VarArrayBuffer</code> struct providing
 * a way to interact with JavaScript ArrayBuffers.
 */


/**
 * @addtogroup Interfaces
 * @{
 */
/**
 * The <code>PPB_VarArrayBuffer</code> interface provides a way to interact
 * with JavaScript ArrayBuffers, which represent a contiguous sequence of
 * bytes. Use <code>PPB_Var</code> to manage the reference count for a
 * <code>VarArrayBuffer</code>. Note that these Vars are not part of the
 * embedding page's DOM, and can only be shared with JavaScript using the
 * <code>PostMessage</code> and <code>HandleMessage</code> functions of
 * <code>pp::Instance</code>.
 */
struct PPB_VarArrayBuffer_1_0 {
  /**
   * Create() creates a zero-initialized <code>VarArrayBuffer</code>.
   *
   * @param[in] size_in_bytes The size of the <code>ArrayBuffer</code> to
   * be created.
   *
   * @return A <code>PP_Var</code> representing a <code>VarArrayBuffer</code>
   * of the requested size and with a reference count of 1.
   */
  struct PP_Var (*Create)(uint32_t size_in_bytes);
  /**
   * ByteLength() retrieves the length of the <code>VarArrayBuffer</code> in
   * bytes. On success, <code>byte_length</code> is set to the length of the
   * given <code>ArrayBuffer</code> var. On failure, <code>byte_length</code>
   * is unchanged (this could happen, for instance, if the given
   * <code>PP_Var</code> is not of type <code>PP_VARTYPE_ARRAY_BUFFER</code>).
   * Note that ByteLength() will successfully retrieve the size of an
   * <code>ArrayBuffer</code> even if the <code>ArrayBuffer</code> is not
   * currently mapped.
   *
   * @param[in] array The <code>ArrayBuffer</code> whose length should be
   * returned.
   *
   * @param[out] byte_length A variable which is set to the length of the given
   * <code>ArrayBuffer</code> on success.
   *
   * @return <code>PP_TRUE</code> on success, <code>PP_FALSE</code> on failure.
   */
  PP_Bool (*ByteLength)(struct PP_Var array, uint32_t* byte_length);
  /**
   * Map() maps the <code>ArrayBuffer</code> in to the module's address space
   * and returns a pointer to the beginning of the buffer for the given
   * <code>ArrayBuffer PP_Var</code>. ArrayBuffers are copied when transmitted,
   * so changes to the underlying memory are not automatically available to
   * the embedding page.
   *
   * Note that calling Map() can be a relatively expensive operation. Use care
   * when calling it in performance-critical code. For example, you should call
   * it only once when looping over an <code>ArrayBuffer</code>.
   *
   * <strong>Example:</strong>
   *
   * @code
   * char* data = (char*)(array_buffer_if.Map(array_buffer_var));
   * uint32_t byte_length = 0;
   * PP_Bool ok = array_buffer_if.ByteLength(array_buffer_var, &byte_length);
   * if (!ok)
   *   return DoSomethingBecauseMyVarIsNotAnArrayBuffer();
   * for (uint32_t i = 0; i < byte_length; ++i)
   *   data[i] = 'A';
   * @endcode
   *
   * @param[in] array The <code>ArrayBuffer</code> whose internal buffer should
   * be returned.
   *
   * @return A pointer to the internal buffer for this
   * <code>ArrayBuffer</code>. Returns <code>NULL</code>
   * if the given <code>PP_Var</code> is not of type
   * <code>PP_VARTYPE_ARRAY_BUFFER</code>.
   */
  void* (*Map)(struct PP_Var array);
  /**
   * Unmap() unmaps the given <code>ArrayBuffer</code> var from the module
   * address space. Use this if you want to save memory but might want to call
   * Map() to map the buffer again later. The <code>PP_Var</code> remains valid
   * and should still be released using <code>PPB_Var</code> when you are done
   * with the <code>ArrayBuffer</code>.
   *
   * @param[in] array The <code>ArrayBuffer</code> to be released.
   */
  void (*Unmap)(struct PP_Var array);
};

typedef struct PPB_VarArrayBuffer_1_0 PPB_VarArrayBuffer;
/**
 * @}
 */

#endif  /* PPAPI_C_PPB_VAR_ARRAY_BUFFER_H_ */

