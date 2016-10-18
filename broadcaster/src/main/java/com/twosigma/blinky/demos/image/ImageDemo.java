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
package com.twosigma.blinky.demos.image;

import java.awt.image.BufferedImage;
import java.io.IOException;
import java.io.InputStream;

import javax.imageio.ImageIO;

import com.twosigma.blinky.demos.Broadcaster;
import com.twosigma.blinky.demos.Demo;
import com.twosigma.blinky.frames.BufferedImageFrame;

public class ImageDemo implements Demo {

	private BufferedImageFrame _frame;

	public ImageDemo(InputStream source) throws IOException {
		BufferedImage ts = ImageIO.read(source);
		_frame = new BufferedImageFrame.Builder(ts).withScale(8, 8).build();
	}

	@Override
	public void run(Broadcaster broadcaster) {
		for (int i = 0; i < 5; i++) {
			broadcaster.draw(_frame, 0, 1000);
		}
	}

}
