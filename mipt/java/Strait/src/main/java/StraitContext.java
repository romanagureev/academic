import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.logging.Logger;

/**
 * All information about strait.
 */
public class StraitContext {
    final public int tunnelCapacity = 5;

    public Semaphore tunnel;
    public ConcurrentHashMap<ShipType, BlockingQueue<Ship>> pierLines;
    public ExecutorService piers;
    public ExecutorService captains;

    private ExecutorService day;
    final private int workingSeconds = 30;
    final private int extraWorkingSeconds = 5;
    public AtomicBoolean workingDay;

    public StraitContext() {
        tunnel = new Semaphore(tunnelCapacity);
        workingDay = new AtomicBoolean(true);

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

    public void startDay() {
        day = Executors.newSingleThreadExecutor();
        day.submit(() -> {
            waitWorkingDay();
            endWorkingDay();
        });
        day.shutdown();
    }

    private void waitWorkingDay() {
        try {
            TimeUnit.SECONDS.sleep(workingSeconds);
            Logger.getLogger(Ship.class.getName()).info("Working seconds ended.");
        } catch (Exception e) {
            Logger.getLogger(Ship.class.getName()).severe(e.getMessage());
        }
    }

    private void endWorkingDay() {
        Logger logger = Logger.getLogger(Ship.class.getName());

        workingDay.set(false);

        piers.shutdown();
        captains.shutdown();
        try {
            while (!piers.awaitTermination(extraWorkingSeconds, TimeUnit.SECONDS)) {
                piers.shutdownNow();
            }
            while (!captains.awaitTermination(extraWorkingSeconds, TimeUnit.SECONDS)) {
                captains.shutdownNow();
            }
        } catch (InterruptedException e) {
            piers.shutdownNow();
            captains.shutdownNow();
            logger.severe(e.getMessage());
        }

        logger.info("Working day ended.");
    }
}
