 # Python : Calculate the sum of an array
def calculate_sum(arr) :
    total = 0
    for num in arr:
    total += num
    return total
 
numbers = [1, 2, 3, 4, 5]
result = calculate_sum (numbers)
print("Sumin Python :", result)
 
# Expected Output: SyntaxError: expected ':'
# The above code will raise a SyntaxError because the function definition 
# is missing a colon at the end of the line.
# Analysis Python parses the file line by line but checks for syntax errors before execution.
# When it encounters the function definition without a colon, it raises a SyntaxError
# indicating that a colon was expected which violates the grammar rules for defining functions in Python,
# causing an immediate crash before any code is executed.:  # <--- Syntax Error: Missing colon
