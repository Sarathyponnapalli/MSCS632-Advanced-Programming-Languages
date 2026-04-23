/*
 * family_tree.pl  -  Family Tree Program
 * Course   : MSCS-632 Advanced Programming Languages
 * Author   : Parthasarathi Ponnapalli
 *
 * A three-generation South Indian family tree.
 *
 * Generation 1 (Grandparents):
 *   Venkataraman  married  Kamakshi   (paternal grandparents)
 *   Subramaniam   married  Meenakshi  (maternal grandparents)
 *
 * Generation 2 (Parents):
 *   Anand    married  Kavitha   -- son of Venkataraman & Kamakshi
 *   Selvam   married  Geetha    -- son of Venkataraman & Kamakshi
 *   Mythili  married  Rajan     -- daughter of Subramaniam & Meenakshi
 *   Vimala   married  Prasanna  -- daughter of Subramaniam & Meenakshi
 *
 * Generation 3 (Children):
 *   Karthik,  Divya    -- children of Anand    & Kavitha
 *   Surya,    Nithya   -- children of Selvam   & Geetha
 *   Arun,     Preethi  -- children of Rajan    & Mythili
 *   Deepak,   Saranya  -- children of Prasanna & Vimala
 */


/* ==============================================================
   SECTION 1  --  BASIC FACTS
   ============================================================== */

/* -- Gender facts --------------------------------------------- */
male(venkataraman).
male(subramaniam).
male(anand).
male(selvam).
male(rajan).
male(prasanna).
male(karthik).
male(surya).
male(arun).
male(deepak).

female(kamakshi).
female(meenakshi).
female(kavitha).
female(geetha).
female(mythili).
female(vimala).
female(divya).
female(nithya).
female(preethi).
female(saranya).

/* -- Parent facts   parent(Parent, Child) --------------------- */

% Generation 1 -> Generation 2
parent(venkataraman, anand).
parent(venkataraman, selvam).
parent(kamakshi,     anand).
parent(kamakshi,     selvam).

parent(subramaniam,  mythili).
parent(subramaniam,  vimala).
parent(meenakshi,    mythili).
parent(meenakshi,    vimala).

% Generation 2 -> Generation 3
parent(anand,    karthik).
parent(anand,    divya).
parent(kavitha,  karthik).
parent(kavitha,  divya).

parent(selvam,   surya).
parent(selvam,   nithya).
parent(geetha,   surya).
parent(geetha,   nithya).

parent(rajan,    arun).
parent(rajan,    preethi).
parent(mythili,  arun).
parent(mythili,  preethi).

parent(prasanna, deepak).
parent(prasanna, saranya).
parent(vimala,   deepak).
parent(vimala,   saranya).


/* ==============================================================
   SECTION 2  --  DERIVED RELATIONSHIP RULES
   ============================================================== */

/* -- Father / Mother ------------------------------------------- */
father(X, Y) :- parent(X, Y), male(X).
mother(X, Y) :- parent(X, Y), female(X).

/* -- Child / Son / Daughter ------------------------------------ */
child(X, Y)    :- parent(Y, X).
son(X, Y)      :- child(X, Y), male(X).
daughter(X, Y) :- child(X, Y), female(X).

/* -- Grandparent ----------------------------------------------
   X is grandparent of Y if X is a parent of Z
   and Z is a parent of Y (two-step chain).                    */
grandparent(X, Y) :-
    parent(X, Z),
    parent(Z, Y).

grandfather(X, Y) :- grandparent(X, Y), male(X).
grandmother(X, Y) :- grandparent(X, Y), female(X).

/* -- Sibling --------------------------------------------------
   X and Y share at least one parent and are not the same person.
   Use setof/3 at query time to deduplicate results.           */
sibling(X, Y) :-
    parent(P, X),
    parent(P, Y),
    X \= Y.

brother(X, Y) :- sibling(X, Y), male(X).
sister(X, Y)  :- sibling(X, Y), female(X).

/* -- Cousin ---------------------------------------------------
   X and Y are cousins if their parents are siblings.          */
cousin(X, Y) :-
    parent(PX, X),
    parent(PY, Y),
    sibling(PX, PY),
    X \= Y.

/* -- Uncle / Aunt --------------------------------------------- */
uncle(X, Y) :-
    parent(P, Y),
    sibling(X, P),
    male(X).

aunt(X, Y) :-
    parent(P, Y),
    sibling(X, P),
    female(X).

/* -- Ancestor (recursive) ------------------------------------
   Base case : a direct parent is an ancestor.
   Recursive : X is an ancestor of Y if X is an ancestor of
               some Z who is a parent of Y.                    */
ancestor(X, Y) :- parent(X, Y).
ancestor(X, Y) :-
    parent(Z, Y),
    ancestor(X, Z).

/* -- Descendant (recursive) ----------------------------------- */
descendant(X, Y) :- ancestor(Y, X).


/* ==============================================================
   SECTION 3  --  SAMPLE QUERIES
   Load: ?- [family_tree].

   Q1  : Children of Anand
         ?- setof(X, child(X, anand), L), member(X, L).
         X = divya ; X = karthik.

   Q2  : Parents of Karthik
         ?- parent(X, karthik).
         X = anand ; X = kavitha.

   Q3  : Grandparents of Karthik
         ?- setof(X, grandparent(X, karthik), L), member(X, L).
         X = kamakshi ; X = venkataraman.

   Q4  : Siblings of Anand
         ?- setof(X, sibling(X, anand), L), member(X, L).
         X = selvam.

   Q5  : Are Karthik and Arun cousins?
         ?- cousin(karthik, arun).
         false.  % different family branches -- not cousins

   Q6  : Are Karthik and Surya cousins?
         ?- cousin(karthik, surya).
         true.   % parents Anand & Selvam are siblings

   Q7  : All cousins of Karthik
         ?- setof(X, cousin(karthik, X), L), member(X, L).
         X = nithya ; X = surya.

   Q8  : Uncle of Karthik
         ?- setof(X, uncle(X, karthik), L), member(X, L).
         X = selvam.

   Q9  : Aunt of Arun
         ?- setof(X, aunt(X, arun), L), member(X, L).
         X = vimala.

   Q10 : Cousins of Arun
         ?- setof(X, cousin(arun, X), L), member(X, L).
         X = deepak ; X = saranya.

   Q11 : All ancestors of Karthik (recursive)
         ?- setof(X, ancestor(X, karthik), L), member(X, L).
         X = anand ; X = kamakshi ; X = kavitha ; X = venkataraman.

   Q12 : All descendants of Venkataraman (recursive)
         ?- setof(X, descendant(X, venkataraman), L), member(X, L).
         X = anand ; X = divya ; X = karthik ; X = nithya ;
         X = selvam ; X = surya.

   Q13 : Grandchildren of Venkataraman
         ?- setof(X, grandparent(venkataraman, X), L), member(X, L).
         X = divya ; X = karthik ; X = nithya ; X = surya.

   Q14 : Is Venkataraman an ancestor of Surya?
         ?- ancestor(venkataraman, surya).
         true.

   Q15 : Female descendants of Subramaniam
         ?- setof(X,(descendant(X,subramaniam),female(X)),L), member(X,L).
         X = mythili ; X = preethi ; X = saranya ; X = vimala.
   ============================================================== */
