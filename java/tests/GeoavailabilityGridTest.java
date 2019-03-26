import geogrid.Coordinates;
import geogrid.GeoavailabilityGrid;
import geogrid.Geohash;
import geogrid.SpatialRange;

import java.io.BufferedReader;
import java.io.FileReader;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.TimeUnit;

public class GeoavailabilityGridTest {
    int dataCounter = 0; // Used for keeping track of insertions for a single iteration through the data set
    int dataSize = 500; // maximum amount of data that should be read from the data set
    int iterations = 0; // current number of iterations through the data set
    int iterationSize = 500; // number of times the data set should be iterated through

    @org.junit.Test
    public void readFileTest() {

        long startTime = System.nanoTime();

        try {
            while (iterations < iterationSize) {
                Map<String, GeoavailabilityGrid> grids = new HashMap<>();
                String filename = "geohashes.txt"; // Put file name here
                BufferedReader br = new BufferedReader(new FileReader(filename));
                String line = br.readLine();
                dataCounter = 0;
                while (line != null && dataCounter < dataSize) {
                    /* Get the first character in the string. We'll create a GeoGrid for
                     * each unique character to split things up. */
                    String prefix = line.substring(0, 1);
                    GeoavailabilityGrid gg = grids.get(prefix);
                    if (gg == null) {
                        grids.put(
                                prefix,
                                new GeoavailabilityGrid(prefix, 10)
                        );
                        gg = grids.get(prefix);
                    }

                    SpatialRange sr = Geohash.decodeHash(line);
                    Coordinates coord = sr.getCenterPoint();
                    //System.out.println(line + " -> " + coord);

                    gg.addPoint(coord);
                    line = br.readLine();
                    dataCounter++;
                }

                iterations++;
                br.close();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        long endTime = System.nanoTime();
        long totalTimeNanoSeconds = endTime - startTime; // nanoseconds
        long totalTimeMilliSeconds = TimeUnit.NANOSECONDS.toMillis(totalTimeNanoSeconds); // milliseconds
        long totalTimeSeconds = TimeUnit.NANOSECONDS.toSeconds(totalTimeNanoSeconds); // Seconds
        System.out.println("Time elapsed (nanoseconds): " + totalTimeNanoSeconds);
        System.out.println("Time elapsed (milliseconds): " + totalTimeMilliSeconds);
        System.out.println("Time elapsed (seconds): " + totalTimeSeconds);
    }

    @org.junit.Test
    public void addPoint() {
        long startTime = System.nanoTime();

        GeoavailabilityGrid geogrid = new GeoavailabilityGrid("8gpcxc4h3n", 12);
        SpatialRange sr = Geohash.decodeHash("8gpfph1fm9");
        Coordinates coord = sr.getCenterPoint();
        geogrid.addPoint(coord);

        long endTime = System.nanoTime();
        long totalTimeNanoSeconds = endTime - startTime; // nanoseconds
        long totalTimeMilliSeconds = TimeUnit.NANOSECONDS.toMillis(totalTimeNanoSeconds); // milliseconds
        long totalTimeSeconds = TimeUnit.NANOSECONDS.toSeconds(totalTimeNanoSeconds); // seconds
        System.out.println("Time elapsed (nano seconds): " + totalTimeNanoSeconds);
        System.out.println("Time elapsed (milli seconds): " + totalTimeMilliSeconds);
        System.out.println("Time elapsed (seconds): " + totalTimeSeconds);
    }

    @org.junit.Test
    public void addPoints() {
        long startTime = System.nanoTime();

        GeoavailabilityGrid geogrid = new GeoavailabilityGrid("8gpcxc4h3n", 12);
        SpatialRange sr = Geohash.decodeHash("8gpfph1fm9");
        Coordinates coord = sr.getCenterPoint();

        SpatialRange sr2 = Geohash.decodeHash("8gpfwr1b0z");
        Coordinates coord2 = sr2.getCenterPoint();

        SpatialRange sr3 = Geohash.decodeHash("8gpgm9cw70");
        Coordinates coord3 = sr2.getCenterPoint();

        geogrid.addPoint(coord);
        geogrid.addPoint(coord2);
        geogrid.addPoint(coord3);

        long endTime = System.nanoTime();
        long totalTimeNanoSeconds = endTime - startTime; // nanoseconds
        long totalTimeMilliSeconds = TimeUnit.NANOSECONDS.toMillis(totalTimeNanoSeconds); // milliseconds
        long totalTimeSeconds = TimeUnit.NANOSECONDS.toSeconds(totalTimeNanoSeconds); // seconds
        System.out.println("Time elapsed (nano seconds): " + totalTimeNanoSeconds);
        System.out.println("Time elapsed (milli seconds): " + totalTimeMilliSeconds);
        System.out.println("Time elapsed (seconds): " + totalTimeSeconds);
    }
}