//#include <Cocoa/Cocoa.h>
//#include <AppKit/NSOpenGL.h>
//#include "speak.h"
//
//extern void speakMe(const char * message){
//	
//	NSString * myWords = [NSString stringWithUTF8String:message];
//	
//	NSSpeechSynthesizer* s = [[NSSpeechSynthesizer alloc] initWithVoice:@"com.apple.speech.synthesis.voice.Alex"];
//	
//	[s startSpeakingString:myWords];
//	
//	[s release];
//}

#import <Cocoa/Cocoa.h>
#import "speak.h"

void speakMe(const char *message) {
	@autoreleasepool {
		// Make sure we run on main thread for AppKit
		if (![NSThread isMainThread]) {
			NSString *text = [NSString stringWithUTF8String:(message ?: "")];
			dispatch_async(dispatch_get_main_queue(), ^{
				speakMe([text UTF8String]);
			});
			return;
		}

		static NSSpeechSynthesizer *synth; // persists across calls under ARC
		if (!synth) {
			synth = [[NSSpeechSynthesizer alloc] initWithVoice:@"com.apple.speech.synthesis.voice.Alex"];
		}

		NSString *text = [NSString stringWithUTF8String:(message ?: "")];
		[synth startSpeakingString:text]; // no manual release under ARC
	}
}
