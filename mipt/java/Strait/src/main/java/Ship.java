import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Semaphore;
import java.util.logging.Logger;

public class Ship implements Runnable {
    final private ShipType type_;
    final private Integer capacity_;
    final private Logger logger_;
    final private Semaphore tunnel_;
    final private BlockingQueue<Ship> pierLine;

    public Ship(ShipType type, Integer capacity, StraitContext ctx) {
        assert (capacity == 10) || (capacity == 50) || (capacity == 100);
        type_ = type;
        capacity_ = capacity;

        tunnel_ = ctx.tunnel;
        pierLine = ctx.pierLines.get(type);

        logger_ = Logger.getLogger(Ship.class.getName());
        logger_.info("A new ship: " + type.name() + ", " + capacity.toString());
    }

    private void goThroughTunnel() throws InterruptedException {
        tunnel_.acquire();
        logger_.info("In the tunnel");
        Thread.sleep(1000);
        tunnel_.release();
        logger_.info("Out of tunnel");
    }

    public void load(Integer rate) throws InterruptedException {
        // Assuming capacity is divisible by rate
        Thread.sleep(1000 * capacity_ / rate);
    }

    @Override
    public void run() {
        logger_.info("Start sailing!");
        try {
            goThroughTunnel();
        } catch (InterruptedException e) {
            logger_.severe(e.getMessage());
        }
        try {
            pierLine.put(this);
        } catch (InterruptedException e) {
            logger_.severe(e.getMessage());
        }
    }

    public Integer getCapacity() {
        return capacity_;
    }

    public ShipType getType() {
        return type_;
    }
}
