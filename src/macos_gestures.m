/*
 * macOS trackpad gestures via AppKit NSEvent phases.
 * Keeps pinch / roll / FOV off the SDL mouse↔touch path so click-drag
 * stays clean, and drops scroll momentum so FOV stops when fingers lift.
 */
#import <AppKit/AppKit.h>
#include "macos_gestures.h"

static id gesture_monitor;

static int phase_is_active(NSEventPhase phase)
{
	if (phase == NSEventPhaseNone)
		return 1; /* physical wheel / legacy events */
	return (phase & (NSEventPhaseBegan | NSEventPhaseChanged)) != 0;
}

void yap_macos_gestures_start(void)
{
	if (gesture_monitor)
		return;

	NSEventMask mask = NSEventMaskMagnify | NSEventMaskRotate |
			   NSEventMaskScrollWheel;
	gesture_monitor = [NSEvent
	    addLocalMonitorForEventsMatchingMask:mask
					 handler:^NSEvent *(NSEvent *e) {
						 switch (e.type) {
						 case NSEventTypeMagnify:
							 if (phase_is_active(
								 e.phase) &&
							     e.magnification !=
								 0.0)
								 yap_on_pinch(
								     (float)e
									 .magnification);
							 break;
						 case NSEventTypeRotate:
							 if (phase_is_active(
								 e.phase) &&
							     e.rotation != 0.0f)
								 yap_on_rotate(
								     e.rotation);
							 break;
						 case NSEventTypeScrollWheel:
							 /* Inertia after lift. */
							 if (e.momentumPhase !=
							     NSEventPhaseNone)
								 break;
							 if (!phase_is_active(
								 e.phase))
								 break;
							 if (e.scrollingDeltaY !=
							     0.0)
								 yap_on_scroll(
								     (float)e
									 .scrollingDeltaY);
							 break;
						 default:
							 break;
						 }
						 return e;
					 }];
}
