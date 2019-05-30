package geogrid;

import java.io.BufferedReader;
import java.io.FileReader;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.TimeUnit;

public class Driver {
    private final long MEGABYTE = 1024L * 1024L;
    int dataCounter = 0; // Used for keeping track of insertions for a single iteration through the data set
    int dataSize = 300000; // maximum amount of data that should be read from the data set
    int iterations = 0; // current number of iterations through the data set. It's worth noting that
    // this variable is also used to index through the runTimes and memoryUsage arrays; this is done because
    // each index of the runTimes and memoryUsage arrays refers to a specific iteration.
    int iterationSize = 1; // number of times the data set should be iterated through
    private Map<String, GeoavailabilityGrid> grids = new HashMap<>();

    public void Driver() { }

    public void readFile(String filename) throws Exception {
        BufferedReader br = new BufferedReader(new FileReader(filename));
        String line = br.readLine();
        while (line != null) {
            /* Get the first character in the string. We'll create a GeoGrid for
             * each unique character to split things up. */
            String prefix = line.substring(0, 1);
            GeoavailabilityGrid gg = grids.get(prefix);
            if (gg == null) {
                grids.put(
                        prefix,
                        new GeoavailabilityGrid(prefix, 16)
                );
                gg = grids.get(prefix);
            }

            SpatialRange sr = Geohash.decodeHash(line);
            Coordinates coord = sr.getCenterPoint();
            System.out.println(line + " -> " + coord);

            gg.addPoint(coord);

            line = br.readLine();
        }
        br.close();
    }

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

    public void benchmark() {
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
                    String prefix = line.substring(0, 2);
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

            System.out.println("-> " + grids.size());
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

    public static void main(String[] args) throws Exception {
        Driver d = new Driver();
        //d.readFile(args[0]);
        d.benchmark();
        System.out.println("Benchmark Complete");
    }

}
