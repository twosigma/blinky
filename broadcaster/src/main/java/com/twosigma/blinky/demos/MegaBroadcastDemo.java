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
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.MalformedURLException;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.Stream;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.twosigma.blinky.demos.gif.GifDemo;
import com.twosigma.blinky.demos.halite.HaliteDemo;
import com.twosigma.blinky.demos.image.ImageDemo;

/**
 * Main application
 */
public class MegaBroadcastDemo {

	public static void main(String[] args)
			throws JsonProcessingException, MalformedURLException, IOException, InterruptedException {
		// "192.168.1.240"
		new MegaBroadcastDemo("255.255.255.255");
	}

	MegaBroadcastDemo(String remoteAddress)
			throws JsonProcessingException, MalformedURLException, IOException, InterruptedException {
		try (DatagramSocket socket = createSocket(remoteAddress)) {
			Broadcaster broadcaster = new Broadcaster(socket);
			broadcaster.start();

			ImageDemo logo = new ImageDemo(
					getClass().getClassLoader().getResourceAsStream("com/twosigma/blinky/demos/image/ts.png"));
			GifDemo animatedLogo = new GifDemo(
					getClass().getClassLoader().getResourceAsStream("com/twosigma/blinky/demos/gif/logopopin.gif"));

			final String path = "com/twosigma/blinky/demos/halite/";

			String[] files = FileUtils.listFiles(path);
			System.out.println(Arrays.toString(files));
			@SuppressWarnings("resource")
			Stream<Demo> haliteDemos = Arrays.<String> stream(files)
					.map(f -> MegaBroadcastDemo.class.getClassLoader().getResourceAsStream(path + f))
					.map(f -> new HaliteDemo(f));

			// mix our logo(s) in between every game
			Stream<Demo> allDemos = haliteDemos;
			List<Demo> flat = allDemos
					.<Demo> flatMap(o -> Stream.<Demo> builder().add(logo).add(o).add(animatedLogo).build())
					.collect(Collectors.toList());
			while (true) {
				for (Iterator<Demo> i = flat.iterator(); i.hasNext();) {
					Demo demo = i.next();
					demo.run(broadcaster);
				}
			}
		}
	}

	private DatagramSocket createSocket(String remoteAddress) throws SocketException, UnknownHostException {
		DatagramSocket socket;
		socket = new DatagramSocket();
		socket.connect(InetAddress.getByName(remoteAddress), 5453);
		socket.setBroadcast(true);
		return socket;
	}
}
