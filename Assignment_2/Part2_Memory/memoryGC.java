class DataObject {
    int id;
    DataObject(int id) { this.id = id; }
    
    // This runs when the Garbage Collector deletes the object
    // (Note: finalize is deprecated in newer Java, but useful for demo)
    protected void finalize() {
        System.out.println("[GC] Garbage Collector freed Object ID: " + this.id);
    }
}

public class MemoryGC {
    public static void main(String[] args) {
        System.out.println("Java Garbage Collection");

        // 1. Allocation: JVM handles the heap request
        DataObject obj = new DataObject(1);
        System.out.println("[Java] Object created.");

        // 2. Dereference: We don't delete. We just cut the link.
        obj = null; 
        System.out.println("[Java] Object reference set to null.");

        // 3. Request Cleanup
        // We ask the JVM to run the Garbage Collector.
        // In a profiler (VisualVM), you would see a "Sawtooth" pattern here.
        System.gc(); 
        
        // Pause to let GC finish (it runs on a separate thread)
        try { Thread.sleep(1000); } catch (Exception e) {}
    }
}