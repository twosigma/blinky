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

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.UnknownHostException;
import java.util.Arrays;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

import com.twosigma.blinky.frames.Frame;

public class Broadcaster extends Thread {

	private DatagramSocket _socket;
	private BlockingQueue<PixelPacket> _pixelQueue = new LinkedBlockingQueue<>();
	private byte[] _buffer = new byte[200];

	public Broadcaster(DatagramSocket socket) {
		_socket = socket;
	}

	@Override
	public void run() {
		sendPackets();
	}
	
	public void draw(Frame frame, int blendTime, int pause) {
		_pixelQueue.add(toPacket(frame, blendTime));
		try {
			Thread.sleep(Math.max(100, pause));
		} catch (InterruptedException e) {
			Thread.currentThread().interrupt();
		}
	}
	
	private PixelPacket toPacket(Frame frame, int blendTime) {
		PixelPacket packet = new PixelPacket(8, 8, blendTime);
		byte[] pixels = packet.getPixels();
		for (int i = 0; i < frame.getWidth(); i++) {
			for (int j = 0; j < frame.getHeight(); j++) {
				int argb = frame.getRGB(i, j);
				int a = argb >> 24;
				int r = argb >> 16 & 0xFF;
				int g = argb >> 8 & 0xFF;
				int b = argb & 0xFF;
				float af = Math.min(1.0f, 1 - (a / 255f));
				int baseOffset = 3 * (i * frame.getWidth() + j);
				pixels[baseOffset + 0] = (byte) (af * r);
				pixels[baseOffset + 1] = (byte) (af * g);
				pixels[baseOffset + 2] = (byte) (af * b);
			}
		}
		return packet;
	}

	private void sendPackets() {
		while (true) {
			try {
				PixelPacket packet = _pixelQueue.poll(250, TimeUnit.MILLISECONDS);
				if (packet != null) {
					try {
						sendPacket(packet);
					} catch (UnknownHostException e) {
						e.printStackTrace();
					}
				}
			} catch (InterruptedException e) {
				Thread.currentThread().interrupt();
			}
		}
	}

	private void sendPacket(PixelPacket packet) throws UnknownHostException {
		int len = packet.pack(_buffer);
		System.out.println(Arrays.toString(_buffer));
		DatagramPacket dgPacket = new DatagramPacket(_buffer, len);
		try {
			_socket.send(dgPacket);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
