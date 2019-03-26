import geogrid.Coordinates;
import geogrid.GeoavailabilityGrid;
import geogrid.Geohash;
import geogrid.SpatialRange;

import java.io.BufferedReader;
import java.io.FileReader;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.TimeUnit;

/*
    Class: GeoavailabilityGridTest
    Purpose: This class contains all of our JUnit tests for
    benchmarking different aspects of our bitmap. Currently,
    we are trying to measure the time it takes to load
    data into the bitmap.
 */
public class GeoavailabilityGridTest {
    int dataCounter = 0; // Used for keeping track of insertions for a single iteration through the data set
    int dataSize = 300000; // maximum amount of data that should be read from the data set
    int iterations = 0; // current number of iterations through the data set
    int iterationSize = 5; // number of times the data set should be iterated through

    /*
        Function: public void readFileTest()
        Purpose: This function is used to read in data from a specified file.
        This function also uses the variables defined at the top of this class
        to determine how much data should be read in from the file. Additionally,
        we iterate over the file n times with n being the value that the variable
        iterationSize is set to (see variables at the top of the class).

        Additionally, this function uses the nanoTime() system call so that
        we can print the total time needed to load the data. nanoseconds, milliseconds,
        and seconds to the console; depending on data set size, one time measure might be
        more insightful than others.
     */
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
        System.out.println("Time elapsed (milliseconds): " + totalTimeMilliSeconds);
    }

    /*
        Function: public void addPoint()
        Purpose: This function adds a single point into the bitmap; it's just a
        very simple test that helps understand the process for inserting values
        into the bitmap.
     */
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

    /*
        Function: public void addPoints()
        Purpose: This is a simple function that just reads in three points. It's
        another simple test to demonstrate the process for inserting values into
        the bitmap.
     */
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