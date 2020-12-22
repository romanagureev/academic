import org.junit.Assert;
import org.junit.Test;

public class ShipGeneratorTests {
    @Test
    public void correctShipArray() {
        StraitContext ctx = new StraitContext();
        ShipGenerator shipGenerator = new ShipGenerator(ctx);
        Ship[] ships = shipGenerator.generate(10);

        Assert.assertEquals(10, ships.length);

        for (Ship ship: ships) {
            Assert.assertNotNull(ship);
            Integer capacity = ship.getCapacity();
            Assert.assertTrue(capacity == 10 || capacity == 50 || capacity == 100);
        }
    }
}
