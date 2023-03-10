---
breadcrumbs:
- - /developers
  - For Developers
- - /developers/accessibility
  - Accessibility for Chromium Developers
- - /developers/accessibility/testing
  - Accessibility Testing
- - /developers/accessibility/testing/automated-testing
  - Accessibility Automated Testing
page_name: ax-inspect
title: Accessibility Inspect Tools
---

These cli tools are designed to help to debug accessibility problems in
applications, primarily in web browsers.

\* `ax_dump_tree` to inspect accessibility tree of application

\* `ax_dump_events` to watch accessibility events fired by application

## Prerequisites

You may be required to enable accessibility on your system. Depending on the
application, you may also be required to run an assistive technology like a
screen reader or use an application-specific runtime flag or flip a preference
on in order to activate accessibility application-wide.

### Mac

1.  Turn on Accessibility for Terminal in Security & Privacy System
            Preferences.
    *   If you connect over SSH, then make sure to turn Accessibility on
                for sshd-keygen-wrapper as well
2.  Eithe
    *   Start VoiceOver (\`CMD+F5\`) or
    *   Use application specific flag/preference
        *   Chrome/Chromium: use \``--force-renderer-accessibility`\`
                    runtime flag

For example, to enable accessibility in Chromium run:

```none
Chromium.app/Contents/MacOS/Chromium --force-renderer-accessibility
```

### Linux

Either

*   Start Orca or
*   Use application specific flag/preference
    *   Chrome/Chromium: use `--force-renderer-accessibility` runtime
                flag

### Windows

Either

*   Start Narrator/NVDA/JAWS or
*   Use application specific flag/preference
    *   Chrome/Chromium: use `--force-renderer-accessibility` runtime
                flag

## ax_dump_tree tool

Helps to inspect accessibility trees of applications. Trees are dumped into
console. To dump an accessible tree, run:

```none
ax_dump_tree <options>
```

## ax_dump_events tool

The tool logs into console accessibility events fired by application. To watch
accessibility events, run:

```none
ax_dump_events <options>
```

## Options

### Selectors

At your convenience the number of pre-defined application selectors are
available:

*   `--chrome` for Chrome browser
*   `--chromium` for Chromium browser
*   `--firefox` for Firefox browser
*   `--edge` for Edge browser (Windows only)
*   `--safari` for Safari browser (Mac only)
*   `--active-tab` to dump a tree of active tab of a selected browser.

You can also specify an application by its title:

```none
ax_dump_tree --pattern=title
```

The pattern string can contain wildcards like \* and ?. Note, you can use
`--pattern` selector in conjunction with pre-defined selectors.

Alternatively you can dump a tree by HWND on Windows:

`--pid=HWND`

Note, to use a hex window handle prefix it with \`0x\`.

Or by application PID on Mac and Linux:

`--pid=process_id`

### Filters

By default a pre-defined set of property filters is applied. If you want to tune
up the output, then use `--filters` option which specifies a file containing
filtering rules:` --filters=absolute_path_to_filters.txt`

The format of the file is a plain text where each line defines a property
filter. The supported property filters are:

*   `@ALLOW` filter means to include the attribute having non empty
            values;
*   `@ALLOW-EMPTY` filter means to include the attribute even if its
            value is empty;
*   `@DENY` filter means to exclude an attribute.

For example, `@ALLOW:AXARIALive` will add `AXARIALive` attributes to the result
tree on mac. Also see `example-tree-filters.txt` in
[tools/accessibility/inspect](https://source.chromium.org/chromium/chromium/src/+/HEAD:tools/accessibility/inspect/example-tree-filters.txt)
for more examples.

### Other options

*   `--help` for help

## Examples

To dump Edge accessible tree on Windows:

```none
out\Default\ax_dump_tree --edge
```

To dump an accessible tree on Mac for a Firefox window having title containing
\`\`mozilla\`\`:

```none
out/Default/ax_dump_tree --firefox --pattern=*mozilla*
```

To watch Chromium accessibility events on Linux:

```none
out/Default/ax_dump_events --chromium
```

## Build

```none
autoninja -C out/Default ax_dump_tree ax_dump_events
```

The command generates `ax_dump_tree` and `ax_dump_events` executables in
\``out/Default`\` directory.
