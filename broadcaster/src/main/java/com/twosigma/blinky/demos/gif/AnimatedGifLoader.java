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

import java.awt.image.BufferedImage;
import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;

import javax.imageio.ImageIO;
import javax.imageio.ImageReader;

import com.twosigma.blinky.frames.BufferedImageFrame;
import com.twosigma.blinky.frames.Frame;

class AnimatedGifLoader {

	public Frame[] loadFrames(InputStream source, int destWidth, int destHeight) throws IOException {
		return Arrays.stream(getImages(source)).map(i -> {
			return new BufferedImageFrame.Builder(i).withScale(destWidth, destHeight).build();
		}).toArray(Frame[]::new);
	}

	/*
	 * Quick and dirty gif loader care of
	 * http://stackoverflow.com/questions/8933893/convert-each-animated-gif-
	 * frame-to-a-separate-bufferedimage#16234122
	 */
	public BufferedImage[] getImages(InputStream source) throws IOException {
		ImageReader reader = ImageIO.getImageReadersByFormatName("gif").next();
		reader.setInput(ImageIO.createImageInputStream(source));
		int numberOfImages = reader.getNumImages(true);
		BufferedImage[] results = new BufferedImage[numberOfImages];
		for (int i = 0; i < numberOfImages; i++)
			results[i] = reader.read(i);
		return results;
	}
}
