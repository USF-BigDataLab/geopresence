package geogrid;

public class RandomDriver {

    public void benchmark() {
        long startTime = System.nanoTime();

        GeoavailabilityGrid gg = new GeoavailabilityGrid("9x", 16);

        for (int i = 0; i < 10000000; ++i) {
            float minLat = gg.getBaseRange().getLowerBoundForLatitude();
            float maxLat = gg.getBaseRange().getUpperBoundForLatitude();
            float randomLat = minLat + (float) Math.random() * (maxLat - minLat);

            float minLon = gg.getBaseRange().getLowerBoundForLongitude();
            float maxLon = gg.getBaseRange().getUpperBoundForLongitude();
            float randomLon = minLon + (float) Math.random() * (maxLon - minLon);

            gg.addPoint(new Coordinates(randomLat, randomLon));
        }

        long endTime = System.nanoTime();
        System.out.println((endTime - startTime) * 1E-6);
    }

    public static void main(String[] args) throws Exception {
        RandomDriver d = new RandomDriver();
        d.benchmark();
    }

}
