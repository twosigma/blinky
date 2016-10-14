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

import com.fasterxml.jackson.databind.node.ArrayNode;
import com.twosigma.blinky.frames.BaseFrame;

class JsonFrame extends BaseFrame {

	private enum Color {
		BLACK(0, 0, 0), BLUE(0, 0, 255), GREEN(0, 255, 0), RED(255, 0, 0), WHITE(255, 255, 255), PURPLE(255, 0,
				255), YELLOW(255, 255, 0);

		private int _value;

		private Color(int r, int g, int b) {
			_value = (r << 16) + (g << 8) + b;
		}

		public int getValue() {
			return _value;
		}
	}

	private ArrayNode _data;

	public JsonFrame(int width, int height, ArrayNode data) {
		super(width, height);
		_data = data;
	}

	@Override
	public int getRGB(int x, int y) {
		return processValue((ArrayNode) _data.get(y).get(x));
	}

	private int processValue(ArrayNode item) {
		int player = item.get(0).asInt();
		int strength = item.get(1).asInt();
		return getColor(player, strength);
	}

	private int getColor(int player, int strength) {
		Color color = Color.values()[player];
		return (strength << 24) + color.getValue();
	}
}
