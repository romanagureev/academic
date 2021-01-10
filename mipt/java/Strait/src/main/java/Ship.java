import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Semaphore;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.logging.Logger;

public class Ship implements Runnable {
    final private ShipType type;
    final private int capacity;
    final private Logger logger;
    final private Semaphore tunnel;
    final private Semaphore loaded;
    final private BlockingQueue<Ship> pierLine;
    final private AtomicBoolean workingDay;

    public Ship(ShipType type, Integer capacity, StraitContext ctx) {
        this.type = type;
        this.capacity = capacity;

        tunnel = ctx.tunnel;
        loaded = new Semaphore(0);
        pierLine = ctx.pierLines.get(type);

        workingDay = ctx.workingDay;

        logger = Logger.getLogger(Ship.class.getName());
        logger.info("A new ship: " + type.name() + ", " + capacity.toString());
    }

    private void goThroughTunnel() throws InterruptedException {
        tunnel.acquire();
        logger.info("In the tunnel");
        Thread.sleep(1000);
        tunnel.release();
        logger.info("Out of tunnel");
    }

    public void load(Integer rate) throws InterruptedException {
        // Assuming capacity is divisible by rate
        Thread.sleep(1000 * capacity / rate);
        loaded.release();
    }

    @Override
    public void run() {
        logger.info("Start sailing!");
        try {
            goThroughTunnel();
            pierLine.put(this);
            loaded.acquire();
        } catch (InterruptedException e) {
            if (!workingDay.get()) {
                logger.info("Working day ended, sailing away.");
                return;
            } else {
                logger.severe(e.getMessage());
            }
        }
        logger.info("Sailed away.");
    }

    public int getCapacity() {
        return capacity;
    }

    public ShipType getType() {
        return type;
    }
}
