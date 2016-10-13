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
package com.twosigma.blinky.demos;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * struct pixel_packet_t { uint32_t magic; uint32_t blend_time; // ms; uint8_t
 * pixels[3 * LED_ROWS * LED_COLS]; };
 * 
 * @author kim
 */
public class PixelPacket {
	private static final int MAGIC = 0x54534c50;
	private int _blendTime = 250;
	private byte[] _pixels;

	public PixelPacket(int width, int height, int blendTime) {
		_pixels = new byte[3 * width * height];
		_blendTime = blendTime;
	}

	public int pack(byte[] dest) {
		ByteBuffer wrap = ByteBuffer.wrap(dest);
		wrap.order(ByteOrder.LITTLE_ENDIAN);
		wrap.clear();
		wrap.position(0);
		wrap.putInt(MAGIC);
		wrap.putInt(_blendTime);
		wrap.put(_pixels, 0, _pixels.length);
		return wrap.position();
	}

	public byte [] getPixels() {
		return _pixels;
	}
}
