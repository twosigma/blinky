/*
 * Copyright (c) Two Sigma Open Source, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.twosigma.blinky.demos.gif;

import java.io.IOException;
import java.io.InputStream;

import com.twosigma.blinky.demos.Broadcaster;
import com.twosigma.blinky.demos.Demo;
import com.twosigma.blinky.frames.Frame;

public class GifDemo implements Demo {

	private Frame[] _frames;

	public GifDemo(InputStream source) throws IOException {
		AnimatedGifLoader gifLoader = new AnimatedGifLoader();
		_frames = gifLoader.loadFrames(source, 8, 8);
	}

	@Override
	public void run(Broadcaster broadcaster) {
		for (int i = 0; i < _frames.length; i++) {
			broadcaster.draw(_frames[i], 0, 100);
		}
	}

}
