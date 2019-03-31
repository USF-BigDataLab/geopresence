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
    private final long MEGABYTE = 1024L * 1024L;
    int dataCounter = 0; // Used for keeping track of insertions for a single iteration through the data set
    int dataSize = 300000; // maximum amount of data that should be read from the data set
    int iterations = 0; // current number of iterations through the data set. It's worth noting that
    // this variable is also used to index through the runTimes and memoryUsage arrays; this is done because
    // each index of the runTimes and memoryUsage arrays refers to a specific iteration.
    int iterationSize = 5; // number of times the data set should be iterated through

    public long bytesToMegabytes(long bytes) {
        return bytes / MEGABYTE;
    }

    // This function calculates the average for our data
    public long calculateAverage(long[] data) {
        long average = 0;
        int len = data.length;

        for (int i = 0; i < len; i++) {
            average = average + data[i];
        }

        average = average / len;
        return average;
    }

    public long calculateTotal(long[] data) {
        int len = data.length;
        long total = 0;

        for (int i = 0; i < len; i++) {
            total = total + data[i];
        }

        return total;
    }

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
        long startTime = System.nanoTime(); // Used for calculating time
        Map<String, GeoavailabilityGrid> grids = new HashMap<>(); // geogrid for testing
        long[] runTimes = new long[100]; // Stores the run time of each iteration
        long[] memoryUsage = new long[100]; // Stores the memory usage of each iteration

        try {
            while (iterations < iterationSize) {
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
                    //System.out.println(line + " -> " + coord); // for testing

                    gg.addPoint(coord);
                    line = br.readLine();
                    dataCounter++;
                }

                Runtime runtime = Runtime.getRuntime(); // Java runtime object
                long endTime = System.nanoTime();
                long totalTimeNanoSeconds = endTime - startTime; // nanoseconds
                long totalTimeMilliSeconds = TimeUnit.NANOSECONDS.toMillis(totalTimeNanoSeconds); // milliseconds
                runTimes[iterations] = totalTimeMilliSeconds;
                runtime.gc(); // Runs the garbage collector
                long memory = runtime.totalMemory() - runtime.freeMemory();
                memoryUsage[iterations] = memory;
                //System.out.println("Time elapsed (milliseconds): " + totalTimeMilliSeconds);

                iterations++;
                br.close();
            }

            //System.out.println("Printing the times from the iteration"); // for testing

            // Here for testing input validation
            /*
            for (int i = 0; i < iterationSize; i++) {
                System.out.println(runTimes[i]);
            }

            for (int i = 0; i < iterationSize; i++) {
                System.out.println(memoryUsage[i]);
            }*/

            long runTimeAverage = calculateAverage(runTimes);
            long memoryUsageAverage = calculateAverage(memoryUsage);
            long memoryUsageTotal = calculateTotal(memoryUsage);
            long endTime = System.nanoTime();
            long allIterationTime = endTime - startTime;
            long allIterationTimeMilliSeconds = TimeUnit.NANOSECONDS.toMillis(allIterationTime);

            System.out.println("RESULTS");
            System.out.println("-----------");
            System.out.println("Average run time:  " + runTimeAverage + " milliseconds");
            System.out.println("Average memory usage: " + memoryUsageAverage + " bytes");
            System.out.println("Total iterations: " + iterationSize);
            System.out.println("Total run time elapsed: " + allIterationTimeMilliSeconds + " milliseconds");
            System.out.println("Total memory used: " + memoryUsageTotal + " bytes");
        } catch (Exception e) {
            e.printStackTrace();
        }
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