fn take_ownership(data: String) {
    println!("[Owner] I now own the memory: {}", data);
} // 'data' is dropped (freed) HERE automatically.

fn main() {
    println!("Rust Ownership Model");

    // 1. Allocation
    // String is allocated on the heap. 's1' is the Owner.
    let s1 = String::from("Hello Memory");
    println!("[Main] s1 owns the string.");

    // 2. Move Semantics (Transfer Ownership)
    // We pass s1 to the function. s1 is now INVALID.
    take_ownership(s1);

    // 3. Compile-Time Safety
    // If you uncomment the line below, the program will NOT compile.
    // Rust prevents "Use After Free" errors before you even run the code.
    
    // println!("{}", s1); // Error: value borrowed here after move
    
    println!("[Main] Program finished safely.");
}