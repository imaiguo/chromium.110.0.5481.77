---
breadcrumbs:
- - /developers
  - For Developers
- - /developers/design-documents
  - Design Documents
page_name: command-dispatch-mac
title: Menus, Hotkeys, & Command Dispatch (Mac)
---

This document covers how toolbar buttons, the main menu bar, and some context
menus are hooked up to the back-end Command infrastructure in Chromium on Mac OS
X.

## Background

Menus are used in three places: the main menu, the toolbar menu, and the context
menu for right clicks. Menus are constructed using the AppKit framework. The two
relevant classes are NSMenu, and NSMenuItem.

Mac OS X has a main menu bar that needs to update based on the front-most window
as well as remain functional when there are no windows open (a situation which
can't happen on Windows and Linux).

## Construction

The class MenuController and its subclasses are used to dynamically generate or
modify menus.

The nib file 'MainMenu.xib' generates the static portion of the main menu.

e.g.

The Bookmarks submenu of the main menu consists of:

1. The static portion of the submenu generated by MainMenu.xib

2. A dynamically updated portion generated by BookmarkMenuBridge.

## Enabling/Disabling Menu Items

The cross-platform (XP) infrastructure uses a CommandUpdater object to represent
the enabled state of a set of commands. Each Browser object has a CommandUpdater
allowing it to maintain unique state that differs from window to window. User
interface views, such as menus or buttons, are responsible for querying the
appropriate CommandUpdater "model" to obtain their enabled state.

In cocoa, the "enabled" state is determined by sending the
validateUserInterfaceItem: selector up the responder chain for each menu item.
Despite the fact that MenuController implements validateUserInterfaceItem:, it
may not be a part of the responder chain. The relevant
validateUserInterfaceItem: implementation will vary from menu to menu. Most menu
items are handled by the BrowserWindowController which asks its corresponding
Browser's CommandUpdater for the enabled state of the command id given by the
item's tag. This also handles toggling commands and updating the item's title to
match current application state.

In the case of buttons, the relevant controller (such as ToolbarController)
registers for changes from the CommandUpdater using a CommandObserverBridge.
When one of the relevant commands changes its state, the bridge sends
enabledStateChangedForCommand:enabled: allowing the controller to update the
enabled state of the button.

For more information, see the [Apple documentation on User Interface
Validation](http://developer.apple.com/mac/library/documentation/Cocoa/Conceptual/UIValidation/UIValidation.html#//apple_ref/doc/uid/10000040).

## Menu Item Dispatch

NSMenuItem has 3 important characteristics: the tag (XP id of the command), the
hotkey/accelerator, and the action/target pair. There are two ways to invoke a
menu item - by clicking on it, or by using the hotkey.

**When a user selects a menu item by clicking on it:**

1.  BrowserCrApplication receives a sendAction:to:from: callback. It
            performs logging & exception handling. It forwards to its parent
            class.
2.  NSApplication invokes the action/target pair.
3.  If the NSMenuItem was created by a MenuController subclass, it may
            have a custom action/target pair. The target is sent the action, and
            processes the response. END
4.  By default, the NSMenuItem has action=commandDispatch:
            target=firstResponder. The tag contains the id of the command. There
            are 2 possible first responders.
5.  If there exists a relevant browser window, the first responder is
            BrowserWindowController, which forwards the invocation to
            BrowserCommandController::ExecuteCommandWithDisposition.
6.  Otherwise, the first responder is AppController, which processes the
            event in commandDispatch:.

**When a user presses the hotkey associated with a menu item (and there exists a
relevant browser window)**, ChromeEventProcessingWindow performKeyEquivalent: is
invoked. It tries to process the hotkey as a extension action, or as a global
keyboard shortcut (such as ctr+tab to move to next tab, see
global_keyboard_shortcuts_mac.mm for more details). If it fails to process the
hotkey, then an event is generated and goes through the same flow from directly
clicking the menu item.

**When a user presses the hotkey associated with a menu item (and there does not
exist a browser window)**, an event is generated and goes through the same flow
from directly clicking the menu item.

There is also a commandDispatchUsingKeyModifiers: for dispatching commands that
do different things based on the state of modifier keys (for example,
command-clicking the "Home" button opens the home page in a new window).

## General Hotkey Dispatch

See figure 1.5 in the [Cocoa Event Handling
Guide](https://developer.apple.com/library/mac/documentation/Cocoa/Conceptual/EventOverview/EventArchitecture/EventArchitecture.html#//apple_ref/doc/uid/10000060i-CH3-SW1).
In Cocoa, an event can only be consumed a single time. The consumer priority is:

1.  Most OS shortcuts. e.g. (command+space) to invoke spotlight, or
            (command+alt+Esc) to invoke "force kill" dialog. Handled directly by
            Cocoa. No NSEvents are sent to Chrome.
2.  Application specific shortcuts, handled through -\[NSResponder
            performKeyEquivalent:\].
3.  Application specific shortcuts from the menu. Handled directly by
            Cocoa.
4.  Remaining OS shortcuts (e.g. command+\`) to shift through windows.
            Handled directly by Cocoa. No callbacks are sent to Chrome.
5.  View specific actions, handled through -\[NSWindow sendEvent:\],
            which becomes -\[NSView wheelScroll:\](or equivalent) at the view
            layer.

Chrome wants to give websites a chance to process hotkeys first, so the process
becomes confusing. When Chrome handles -\[NSResponder performKeyEquivalent:\],
it marks the hotkey as consumed. The NSEvent never reaches steps 3 through 5.
Chrome sends the event asynchronously to the renderer, which returns an
asynchronous message indicating whether the event was consumed. If the event was
not consumed, a new event is generated and passed through the entire cycle
again, via -\[NSApp sendEvent:\]. The second time through, a flag is set so that
-\[NSResponder performKeyEquivalent:\] does not consume the event.

One important fact is that if Chrome chooses to not consume the event in step
(2), NSMenu shortcuts are guaranteed to attempt to consume the event (step 3).
Furthermore, the remaining OS shortcuts in step (4) can be modified by the user.
Therefore, there is no well established method for determining whether the
system would attempt to consume an event without also letting Chrome's NSMenu
shortcuts attempt to consume the event.
