public class Main {
    public static void main(String[] args) {
        StraitContext strait = new StraitContext();
        ShipGenerator shipGenerator = new ShipGenerator(strait);

        Ship[] ships = shipGenerator.generate(10);
        start(ships, strait);
    }

    /**
     * Start ships sailing.
     * @param ships
     * @param strait
     */
    public static void start(Ship[] ships, StraitContext strait) {
        for (Ship ship: ships) {
            strait.captains.submit(ship);
        }
    }
}
