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

	private InputStream _source;

	public GifDemo(InputStream source) {
		_source = source;
	}

	@Override
	public void run(Broadcaster broadcaster) {
		AnimatedGifLoader gifLoader = new AnimatedGifLoader();
		try {
			Frame[] frames = gifLoader.loadFrames(_source, 8, 8);
			while (true) {
				for (int i = 0; i < frames.length; i++) {
					broadcaster.draw(frames[i], 0, 100);
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

}
