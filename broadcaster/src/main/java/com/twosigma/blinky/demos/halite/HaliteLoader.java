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
import java.util.Iterator;
import java.util.NoSuchElementException;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ArrayNode;
import com.twosigma.blinky.frames.BufferedImageFrame;
import com.twosigma.blinky.frames.Frame;

class HaliteLoader {

	class FrameIterator implements Iterator<Frame> {

		private final ArrayNode _framesNode;
		private final int _length;
		private final int _destWidth;
		private final int _destHeight;
		private final int _sourceWidth;
		private final int _sourceHeight;
		private int _i;

		public FrameIterator(int sourceWidth, int sourceHeight, int destWidth, int destHeight, ArrayNode framesNode) {
			_framesNode = framesNode;
			_length = framesNode.size();
			_sourceWidth = sourceWidth;
			_sourceHeight = sourceHeight;
			_destWidth = destWidth;
			_destHeight = destHeight;
		}

		@Override
		public boolean hasNext() {
			return _i < _length;
		}

		@Override
		public Frame next() {
			if (hasNext()) {
				Frame frame = new JsonFrame(_sourceWidth, _sourceHeight, (ArrayNode) _framesNode.get(_i++));
				if (Math.max(frame.getHeight(), frame.getWidth()) > 8) {
					frame = new BufferedImageFrame.Builder(frame).withScale(_destWidth, _destHeight).build();
				}
				return frame;
			}
			throw new NoSuchElementException();
		}

		public void reset() {
			_i = 0;
		}

	}

	FrameIterator loadFrames(InputStream data, int destWidth, int destHeight)
			throws JsonProcessingException, IOException {
		ObjectMapper mapper = new ObjectMapper();
		JsonNode node = mapper.readTree(data);
		// int height = node.get("height").asInt();
		// int width = node.get("width").asInt();
		ArrayNode framesNode = (ArrayNode) node.get("frames");
		return new FrameIterator(node.get("width").asInt(), node.get("width").asInt(), destWidth, destHeight,
				framesNode);
	}
}
