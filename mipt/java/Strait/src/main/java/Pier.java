import java.util.concurrent.BlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.logging.Logger;

public class Pier implements Runnable {
    final private ShipType type;
    final private int rate = 10;
    final private int pollTimeMs = 100;
    final private AtomicBoolean workingDay;
    final private Logger logger;
    final private BlockingQueue<Ship> line;

    public Pier(ShipType type, StraitContext ctx) {
        this.type = type;
        workingDay = ctx.workingDay;
        line = ctx.pierLines.get(type);
        logger = Logger.getLogger(Pier.class.getName());
    }

    @Override
    public void run() {
        logger.info("Pier started working.");
        while (workingDay.get()) {
            try {
                Ship ship = line.poll(pollTimeMs, TimeUnit.MILLISECONDS);
                if (ship == null) {
                    continue;
                }
                logger.info("A new ship arrived!");
                ship.load(rate);
                logger.info(type.name() + " loaded.");
            } catch (InterruptedException e) {
                if (!workingDay.get()) {
                    logger.info("Working day ended, going home.");
                    return;
                } else {
                    logger.severe(e.getMessage());
                }
            }
        }
    }

    public ShipType getType() {
        return type;
    }
}
