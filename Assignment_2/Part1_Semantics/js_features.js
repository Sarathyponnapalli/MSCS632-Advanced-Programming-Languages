function analyzeJSFeatures() {
    console.log("JavaScript Feature Analysis");

    // 1. Feature: Weak Typing (Type Coercion)
    // JS tries to prevent crashes by implicitly converting types.
    let num = 10;
    let text = "5";
    
    // Instead of Math (15), JS does String Concatenation ("105")
    let result = num + text; 
    console.log(`[Weak Typing] 10 + "5" = ${result} (String)`); 

    // 2. Feature: Scope (var vs let)
    // 'var' ignores blocks (like Python), 'let' respects blocks (like C++)
    if (true) {
        var oldVar = "I ignore the scope of the block";
        let newLet = "I respect the scope of the block";
    }
    
    console.log(`[Scope] 'var' is accessible: ${oldVar}`);
    
    try {
        console.log(newLet); // This will crash
    } catch (e) {
        console.log(`[Scope] 'let' is not accessible (Error: ${e.name})`);
    }
}

analyzeJSFeatures();