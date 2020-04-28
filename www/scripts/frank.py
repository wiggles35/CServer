#!/usr/bin/env python3

import cgi

print('HTTP/1.0 200 OK')
print('Content-Type: text/html')
print()

form = cgi.FieldStorage()

correct = 0
flag = False

if 'iscool' in form:
    flag = True
    if (form['iscool'].value == 'frank'):
        correct = correct + 1
if 'fav' in form:
    flag = True
    if (form['fav'].value == 'frank' or form['fav'].value == 'ryan'):
        correct = correct + 1

if flag:
    print(f"<h1>Score: {correct} / 2</h2>")


print('''
<form>
    <p>Select the coolest cat in the club:\n</p>
    <input type="radio" id="frank" name="iscool" value="frank">
    <label for="frank">Frank</label><br>
    <input type="radio" id="notfrank" name="iscool" value="notfrank">
    <label for="notfrank">Not Frank</label><br>
    <input type="radio" id="defnotfrank" name="iscool" value="defnotfrank">
    <label for="definitely not frank">Definitely Not Frank</label>

    <p>Select Bui's favorite student:\n</p>
    <input type="radio" id="frank" name="fav" value="frank">
    <label for="frank">Frank</label><br>
    <input type="radio" id="rpmccarte" name="fav" value="rpmccarte">
    <label for="rpmccarte">rpmccarte</label><br>
    <input type="radio" id="db29" name="fav" value="db29">
    <label for="db29">db29</label><br>
    <input type="radio" id="ryan" name="fav" value="ryan">
    <label for="ryan">Ryan</label><br>


    <input type="submit">
</form>
''')
