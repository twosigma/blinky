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
import java.net.URI;
import java.net.URISyntaxException;
import java.nio.file.FileSystem;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Collections;

public final class FileUtils {

	public static String[] listFiles(String path) throws URISyntaxException, IOException {
		URI uri = FileUtils.class.getClassLoader().getResource(path).toURI();
		if (uri.getScheme().equals("jar")) {
			FileSystem fileSystem = FileSystems.newFileSystem(uri, Collections.<String, Object> emptyMap());
			return Files.walk(fileSystem.getPath(path), 1).map(p -> p.toString().substring(1))
					.filter(s -> s.endsWith(".hlt")).toArray(String[]::new);
		} else {
			Path massagedPath = Paths.get(uri);
			Path root = Paths.get(FileUtils.class.getClassLoader().getResource("").toURI());
			return Files.walk(massagedPath, 1).map(p -> root.relativize(p).toString())
					.filter(s -> s.endsWith(".hlt")).toArray(String[]::new);
		}
	}
}
