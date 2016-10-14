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
package com.twosigma.blinky.demos.halite;

import java.io.IOException;
import java.io.InputStream;

import com.twosigma.blinky.demos.Broadcaster;
import com.twosigma.blinky.demos.Demo;
import com.twosigma.blinky.demos.halite.HaliteLoader.FrameIterator;
import com.twosigma.blinky.frames.Frame;
import com.twosigma.blinky.frames.SolidFrame;

public class HaliteDemo implements Demo {

	private InputStream _stream;
	private FrameIterator _iterator;

	public HaliteDemo(InputStream stream) {
		_stream = stream;
		HaliteLoader loader = new HaliteLoader();
		try {
			_iterator = loader.loadFrames(_stream, 8, 8);
		} catch (IOException e) {
			throw new IllegalArgumentException(e);
		}
	}

	@Override
	public void run(Broadcaster broadcaster) {
		_iterator.reset();
		Frame image = null;
		while (_iterator.hasNext()) {
			final long startTime = System.currentTimeMillis();
			image = _iterator.next();
			final int wait = Math.max(0, 250 - (int) (System.currentTimeMillis() - startTime));
			broadcaster.draw(image, 750, wait);
		}
		if (image != null) {
			writeWin(broadcaster, image);
		}
	}

	/*
	 * Unfortunately the data format doesn't list the winner of a game. Figure
	 * out the winner based on the first located color in the board greater than
	 * some minimum threshold. Even tho there should only be one color left it
	 * may be that some artifact of blending has resulted in a less than
	 * enthusiastic colour.
	 */
	private void writeWin(Broadcaster broadcaster, Frame last) {
		for (int i = 0; i < last.getWidth(); i++) {
			for (int j = 0; j < last.getHeight(); j++) {
				int value = last.getRGB(i, j);
				if (strongValue(value)) {
					SolidFrame on = new SolidFrame(last.getWidth(), last.getHeight(), value);
					SolidFrame off = new SolidFrame(last.getWidth(), last.getHeight(), 0);
					for (int k = 0; k < 5; k++) {
						broadcaster.draw(on, 500, 250);
						broadcaster.draw(off, 500, 250);
					}
					return;
				}
			}
		}
	}

	/*
	 * Answers whether or not a given value is strong enough to be considered a
	 * winner.
	 */
	private boolean strongValue(int value) {
		int r = value >> 16 & 0xFF;
		if (r > 192) {
			return true;
		}
		int g = value >> 8 & 0xFF;
		if (g > 192) {
			return true;
		}
		int b = value & 0xFF;
		if (b > 192) {
			return true;
		}
		return false;
	}
}
