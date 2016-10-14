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

public abstract class BaseFrame implements Frame {

	private int _width;
	private int _height;

	public BaseFrame(int width, int height) {
		_width = width;
		_height = height;
	}

	@Override
	public int getWidth() {
		return _width;
	}

	@Override
	public int getHeight() {
		return _height;
	}
}
