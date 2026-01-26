// Error: Missing closing parenthesis ')' in function definition
function calculateSum(arr { 
    let total = 0;
    for let num of arr {  //Invalid for loop syntax
        total += num;
    }
    return total;
}

let numbers = [1, 2, 3, 4, 5];
let result = calculateSum(numbers);
console.log("Sum in JavaScript:", result);

//Expected Output: SyntaxError: Unexpected token '{'
// The above code will raise a SyntaxError because of the incorrect use of '{' instead of '(' in the function definition and for loop.
