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
package com.twosigma.blinky.frames;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.image.BufferedImage;

public class BufferedImageFrame extends BaseFrame {

	public static class Builder {
		private int _width;
		private int _height;
		private int _scaleMode = Image.SCALE_AREA_AVERAGING;
		private BufferedImage _sourceImage;
		private Frame _sourceFrame;

		public Builder(Frame source) {
			_sourceFrame = source;
			_width = source.getWidth();
			_height = source.getHeight();
		}

		public Builder(BufferedImage source) {
			_sourceImage = source;
			_width = source.getWidth();
			_height = source.getHeight();
		}

		public Builder withScale(int width, int height, int scale) {
			_width = width;
			_height = height;
			_scaleMode = scale;
			return this;
		}
		

		public Builder withScale(int destWidth, int destHeight) {
			return withScale(destWidth, destHeight, Image.SCALE_AREA_AVERAGING);
		}

		private BufferedImage toImage(Frame source) {
			BufferedImage image = new BufferedImage(source.getWidth(), source.getHeight(), BufferedImage.TYPE_INT_ARGB);
			Graphics2D g = image.createGraphics();
			for (int i = 0; i < source.getWidth(); i++) {
				for (int j = 0; j < source.getHeight(); j++) {
					processValue(g, i, j, source.getRGB(i, j));
				}
			}

			g.dispose();

			return image;
		}

		private void processValue(Graphics2D g, int col, int row, int value) {
			g.setPaint(new Color(value, true));
			g.drawLine(col, row, col, row);
		}

		private BufferedImage optionalScale(BufferedImage image) {
			BufferedImage result = image;
			if (_width != image.getWidth() || _height != image.getHeight()) {
				result = scale(image);
			}
			return result;
		}

		private BufferedImage scale(BufferedImage image) {
			Image scaled = image.getScaledInstance(_width, _height, _scaleMode);
			BufferedImage result = new BufferedImage(_width, _height, BufferedImage.TYPE_INT_ARGB);
			result.getGraphics().drawImage(scaled, 0, 0, null);
			return result;
		}

		public BufferedImageFrame build() {
			return new BufferedImageFrame(_width, _height,
					optionalScale(_sourceImage != null ? _sourceImage : toImage(_sourceFrame)));
		}
	}

	private final BufferedImage _image;

	private BufferedImageFrame(int targetWidth, int targetHeight, BufferedImage source) {
		super(targetWidth, targetHeight);
		_image = source;
	}

	@Override
	public int getRGB(int x, int y) {
		return _image.getRGB(x, y);
	}
}
