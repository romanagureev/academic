import java.util.concurrent.*;
import java.util.logging.Logger;

/**
 * All information about strait.
 */
public class StraitContext {
    final public Integer tunnelCapacity = 5;

    public Semaphore tunnel;
    public ConcurrentHashMap<ShipType, BlockingQueue<Ship>> pierLines;
    public ExecutorService piers;
    public ExecutorService captains;

    public StraitContext() {
        tunnel = new Semaphore(tunnelCapacity);

        pierLines = new ConcurrentHashMap<>();
        for (ShipType type: ShipType.values()) {
            pierLines.put(type, new SynchronousQueue<>());
        }

        piers = Executors.newFixedThreadPool(pierLines.size());
        captains = Executors.newCachedThreadPool();

        Logger logger = Logger.getLogger(StraitContext.class.getName());
        logger.info("Starting generating piers.");
        for (ShipType type: ShipType.values()) {
            Pier pier = new Pier(type, this);
            piers.submit(pier);
        }
        logger.info("All piers generated.");
    }
}
