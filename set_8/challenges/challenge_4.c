// // ------------------------------------------------------------

// 60. Single-Coordinate Ladders and Insecure Twists

// All our hard work is about to pay some dividends. Here's a list of
// cool-kids jargon you'll be able to deploy after completing this
// challenge:

// * Montgomery curve
// * single-coordinate ladder
// * isomorphism
// * birational equivalence
// * quadratic twist
// * trace of Frobenius

// Not that you'll understand it all; you won't. But you'll at least be
// able to silence crypto-dilettantes on Twitter.

// Now, to the task at hand. In the last problem, we implemented ECDH
// using a short Weierstrass curve form, like this:

//     y^2 = x^3 + a*x + b

// For a long time, this has been the most popular curve form. The NIST
// P-curves standardized in the 90s look like this. It's what you'll see
// first in most elliptic curve tutorials (including this one).

// We can do a lot better. Meet the Montgomery curve:

//     B*v^2 = u^3 + A*u^2 + u

// Although it's almost as old as the Weierstrass form, it's been buried
// in the literature until somewhat recently. The Montgomery curve has a
// killer feature in the form of a simple and efficient algorithm to
// compute scalar multiplication: the Montgomery ladder.

// Here's the ladder:

//     function ladder(u, k):
//         u2, w2 := (1, 0)
//         u3, w3 := (u, 1)
//         for i in reverse(range(bitlen(p))):
//             b := 1 & (k >> i)
//             u2, u3 := cswap(u2, u3, b)
//             w2, w3 := cswap(w2, w3, b)
//             u3, w3 := ((u2*u3 - w2*w3)^2,
//                        u * (u2*w3 - w2*u3)^2)
//             u2, w2 := ((u2^2 - w2^2)^2,
//                        4*u2*w2 * (u2^2 + A*u2*w2 + w2^2))
//             u2, u3 := cswap(u2, u3, b)
//             w2, w3 := cswap(w2, w3, b)
//         return u2 * w2^(p-2)

// You are not expected to understand this.

// No, really! Most people don't understand it. Instead, they visit the
// Explicit-Formulas Database (https://www.hyperelliptic.org/EFD/), the
// one-stop shop for state-of-the-art ECC implementation techniques. It's
// like cheat codes for elliptic curves. Worth visiting for the
// bibliography alone.

// With that said, we should try to demystify this a little bit. Here's
// the CliffsNotes:

// 1. Points on a Montgomery curve are (u, v) pairs, but this function
//    only takes u as an input. Given *just* the u coordinate of a point
//    P, this function computes *just* the u coordinate of k*P. Since we
//    only care about u, this is a single-coordinate ladder.

// 2. So what the heck is w? It's part of an alternate point
//    representation. Instead of a coordinate u, we have a coordinate
//    u/w. Think of it as a way to defer expensive division (read:
//    inversion) operations until the very end.

// 3. cswap is a function that swaps its first two arguments (or not)
//    depending on whether its third argument is one or zero. Choosy
//    implementers choose arithmetic implementations of cswap, not
//    branching ones.

// 4. The core of the inner loop is a differential addition followed by a
//    doubling operation. Differential addition means we can add two
//    points P and Q only if we already know P - Q. We'll take this
//    difference to be the input u and maintain it as an invariant
//    throughout the ladder. Indeed, our two initial points are:

//        u2, w2 := (1, 0)
//        u3, w3 := (u, 1)

//    Representing the identity and the input u.

// 5. The return statement performs the modular inversion using a trick
//    due to Fermat's Little Theorem:

//        a^p     = a    mod p
//        a^(p-1) = 1    mod p
//        a^(p-2) = a^-1 mod p

// 6. A consequence of the Montgomery ladder is that we conflate (u, v)
//    and (u, -v). But this encoding also conflates zero and
//    infinity. Both are represented as zero. Note that the usual
//    exceptional case where w = 0 is handled gracefully: our trick for
//    doing the inversion with exponentiation outputs zero as expected.

//    This is fine: we're still working in a subgroup of prime order.

// Go ahead and implement the ladder. Remember that all computations are
// in GF(233970423115425145524320034830162017933).

// Oh yeah, the curve parameters. You might be thinking that since we're
// switching to a new curve format, we also need to pick out a whole new
// curve. But you'd be totally wrong! It turns out that some short
// Weierstrass curves can be converted into Montgomery curves.

// This is because all finite cyclic groups with an equal number of
// elements share a kind of equivalence we call "isomorphism". It makes
// sense, if you think about it - if the order is the same, all the same
// subgroups will be present, and in the same proportions.

// So all we need to do is:

// 1. Find a Montgomery curve with an equal order to our curve.

// 2. Figure out how to map points back and forth between curves.

// You can perform this conversion algebraically. But it's kind of a
// pain, so here you go:

//     v^2 = u^3 + 534*u^2 + u

// Through cunning and foresight, I have chosen this curve specifically
// to have a really simple map between Weierstrass and Montgomery
// forms. Here it is:

//     u = x - 178
//     v = y

// Which makes our base point:

//     (4, 85518893674295321206118380980485522083)

// Or, you know. Just 4.

// Anyway, implement the ladder. Verify ladder(4, n) = 0. Map some points
// back and forth between your Weierstrass and Montgomery representations
// and verify them.

// One nice thing about the Montgomery ladder is its lack of special
// cases. Specifically, no special handling of: P1 = O; P2 = O; P1 = P2;
// or P1 = -P2. Contrast that with our Weierstrass addition function and
// its battalion of ifs.

// And there's a security benefit, too: by ignoring the v coordinate, we
// take away a lot of leeway from the attacker. Recall that the ability
// to choose arbitrary (x, y) pairs let them cherry-pick points from any
// curve they can think of. The single-coordinate ladder robs the
// attacker of that freedom.

// But hang on a tick! Give this a whirl:

//     ladder(76600469441198017145391791613091732004, 11)

// What the heck? What's going on here?

// Let's do a quick sanity check. Here's the curve equation again:

//     v^2 = u^3 + 534*u^2 + u

// Plug in u and take the square root to recover v.

// You should detect that something is quite wrong. This u does not
// represent a point on our curve! Not every u does.

// This means that even though we can only submit one coordinate, we
// still have a little bit of leeway to find invalid
// points. Specifically, an input u such that u^3 + 534*u^2 + u is not a
// quadratic residue can never represent a point on our curve. So where
// the heck are we?

// The other curve we're on is a sister curve called a "quadratic twist",
// or simply "the twist". There is actually a whole family of quadratic
// twists to our curve, but they're all isomorphic to each
// other. Remember that that means they have the same number of points,
// the same subgroups, etc. So it doesn't really matter which particular
// twist we use; in fact, we don't even need to pick one.

// We're mostly interested in the subgroups present on the twist, which
// means we need to know how many points it contains. Fortunately, it
// turns out to be easier to count the combined set of points on the
// curve and its twist at the same time. Let's do it:

// 1. For every nonzero u up to the modulus p, if u^3 + A*u^2 + u is a
//    square in GF(p), there are two points on the original curve.

// 2. If the above sum is a nonsquare in GF(p), there are two points on
//    the twisted curve.

// It should be clear that these add up to 2*(p-1) points in total, since
// there are p-1 nonzero integers in GF(p) and two points for each. Let's
// continue:

// 3. Both the original curve and its twist have a point (0, 0). This is
//    just a regular point, not the group identity.

// 4. Both the original curve and its twist have an abstract point at
//    infinity which serves as the group identity.

// So we have 2*p + 2 points across both curves. Since we already know
// how many points are on the original curve, we can easily calculate the
// order of the twist.

// If Alice chose a curve with an insecure twist, i.e. one with a
// partially smooth order, then some doors open back up for Eve. She can
// choose low-order points on the twisted curve, send them to Alice, and
// perform the invalid-curve attack as before.

// The only caveat is that she won't be able to recover the full secret
// using off-curve points, only a fraction of it. But we know how to
// handle that.

// So:

// 1. Calculate the order of the twist and find its small factors. This
//    one should have a bunch under 2^24.

// 2. Find points with those orders. This is simple:

//    a. Choose a random u mod p and verify that u^3 + A*u^2 + u is a
//       nonsquare in GF(p).

//    b. Call the order of the twist n. To find an element of order q,
//       calculate ladder(u, n/q).

// 3. Send these points to Alice to recover portions of her secret.

// 4. When you've exhausted all the small subgroups in the twist, recover
//    the remainder of Alice's secret with the kangaroo attack.

// HINT: You may come to notice that k*u = -k*u, resulting in a
// combinatorial explosion of potential CRT outputs. Try sending extra
// queries to narrow the range of possibilities.
