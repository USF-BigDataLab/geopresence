package geogrid;

import java.io.BufferedReader;
import java.io.FileReader;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.Map;
import java.util.stream.Stream;

public class Driver {

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
                        new GeoavailabilityGrid(prefix, 10)
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

    public static void main(String[] args) throws Exception {

        Driver d = new Driver();
        d.readFile(args[0]);

    }

}
