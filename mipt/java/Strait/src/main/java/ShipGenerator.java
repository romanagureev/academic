import java.util.ArrayList;
import java.util.Arrays;
import java.util.Random;
import java.util.logging.Logger;

public class ShipGenerator {
    final private StraitContext ctx_;
    final private Logger logger_;

    public ShipGenerator(StraitContext ctx) {
        ctx_ = ctx;
        logger_ = Logger.getLogger(ShipGenerator.class.getName());
    }

    /**
     * Generate random ships.
     * @param ships_cnt quantity of ships to generate.
     * @return
     */
    public Ship[] generate(Integer ships_cnt) {
        Ship[] ships = new Ship[ships_cnt];

        Integer[] capacities = {10, 50, 100};
        ArrayList<ShipType> types = new ArrayList<>(Arrays.asList(ShipType.values()));
        Random random = new Random();

        for (int i = 0; i < ships_cnt; ++i) {
            ships[i] = new Ship(types.get(random.nextInt(3)),
                                capacities[random.nextInt(3)],
                                ctx_);
        }

        logger_.info("Generated " + ships_cnt.toString() + " ships.");
        return ships;
    }
}
