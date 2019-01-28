# geopresence

The main idea behind this work is to create a geospatial index that runs on IoT/low-power devices. Location-based services, weather tracking, mapping, and GIS are all applicable domains. For a simplistic example, imagine air quality and weather monitoring devices placed on the streetlights in a city; rather than querying weather.com for the current weather in the Presidio or Mission District, why not just ask the streetlight across the street from the restaurant you're going to?

Here is some previous work in this area:

* https://www.cs.usfca.edu/~mmalensek/publications/malensek2013polygon.pdf
* https://www.cs.usfca.edu/~mmalensek/publications/malensek2014geometry.pdf

This approach works fine for small spatial regions, but begins to slow down when (1) resolution is increased, and (2) the size of the bitmaps and/or queries becomes too large. It also has no mechanism for dealing with high density points.

Info about bitmaps: https://en.wikipedia.org/wiki/Bit_array

The previous approaches use EWAH compressed bitmaps. We can likely leverage the newer, better [Roaring Bitmaps](https://roaringbitmap.org) to already gain an advantage over the previous approach. We will also implement this project in C so it will be more lightweight than the Java-based implementation.
