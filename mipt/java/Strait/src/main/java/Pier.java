import java.util.concurrent.BlockingQueue;
import java.util.logging.Logger;

public class Pier implements Runnable {
    final private ShipType type_;
    final private Integer rate_ = 10;
    final private Logger logger_;
    final private BlockingQueue<Ship> line_;

    public Pier(ShipType type, StraitContext ctx) {
        type_ = type;
        line_ = ctx.pierLines.get(type);
        logger_ = Logger.getLogger(Pier.class.getName());
    }

    @Override
    public void run() {
        logger_.info("Pier started working.");
        while (true) {
            try {
                Ship ship = line_.take();
                logger_.info("A new ship arrived!");
                ship.load(rate_);
                logger_.info(type_.name() + " loaded.");
            } catch (InterruptedException e) {
                logger_.severe(e.getMessage());
            }
        }
    }

    public ShipType getType() {
        return type_;
    }
}
