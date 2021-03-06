
// // ------------------------------------------------------------

// 59. Elliptic Curve Diffie-Hellman and Invalid-Curve Attacks

// I'm not going to show you any graphs - if you want to see one, you can
// find them in, like, every other elliptic curve tutorial on the
// internet. Personally, I've never been able to gain much insight from
// them.

// They're also really hard to draw in ASCII.

// The key thing to understand about elliptic curves is that they're a
// setting analogous in many ways to one we're more familiar with, the
// multiplicative integers mod p. So if we learn how certain primitive
// operations are defined, we can reason about them using a lot of tools
// we already have in our utility belts.

// Let's dig in. An elliptic curve E is just an equation like this:

//     y^2 = x^3 + a*x + b

// The choice of the a and b coefficients defines the curve.

// The elements in our group are going to be (x, y) coordinates
// satisfying the curve equation. Now, there are infinitely many pairs
// like that on the curve, but we only want to think about some of
// them. We'll trim our set of points down by considering the curve in
// the context of a finite field.

// For the moment, it's not too important to know what a finite field
// is. You can basically just think of it as "integers mod p" with all
// the usual operations you expect: multiplication, division (via modular
// inversion), addition, and subtraction.

// We'll use the notation GF(p) to talk about a finite field of size
// p. (The "GF" is for "Galois field", another name for a finite field.)
// When we take a curve E over field GF(p) (written E(GF(p))), what we're
// saying is that only points with both x and y in GF(p) are valid.

// For example, (3, 6) might be a valid point in E(GF(7)), but it
// wouldn't be a valid point in E(GF(5)); 6 is not a member of GF(5).

// (3, 4.7) wouldn't be a valid point on either curve, since 4.7 is not
// an integer and thus not a member of either field.

// What about (3, -1)? This one is on the curve, but remember we're in
// some GF(p). So in GF(7), -1 is actually 6. That means (3, -1) and (3,
// 6) are the same point. In GF(5), -1 is 4, so blah blah blah you get
// what I'm saying.

// Okay: if these points are going to form a group analogous to the
// multiplicative integers mod p, we need to have an analogous set of
// primitive functions to work with them.

// 1. In the multiplicative integers mod p, we combined two elements by
//    multiplying them together and taking the remainder modulo p.

//    We combine elliptic curve points by adding them. We'll talk about
//    what that means in a hot second.

// 2. We used 1 as a multiplicative identity: y * 1 = y for all y.

//    On an elliptic curve, we define the identity O as an abstract
//    "point at infinity" that doesn't map to any actual (x, y)
//    pair. This might feel like a bit of a hack, but it works.

//    On the curve, we have the straightforward rule that P + O = P for
//    all P.

//    In your code, you can just write something like O := object(),
//    since it only ever gets used in pointer comparisons. Or you can use
//    some sentinel coordinate that doesn't satisfy the curve equation;
//    (0, 1) is popular.

// 3. We had a modinv function to invert an integer mod p. This acted as
//    a stand-in for division. Given y, it finds x such that y * x = 1.

//    Inversion is way easier in elliptic curves. Just flip the sign on
//    y, and remember that we're in GF(p):

//        invert((x, y)) = (x, -y) = (x, p-y)

//    Just like with multiplicative inverses, we have this rule on
//    elliptic curves:

//        P + (-P) = P + invert(P) = O

// Incidentally, these primitives, along with a finite set of elements,
// are all we need to define a finite cyclic group, which is all we need
// to define the Diffie-Hellman function. Not important to understand the
// abstract jargon, just FYI.

// Let's talk about addition. Here it is:

//     function add(P1, P2):
//         if P1 = O:
//             return P2

//         if P2 = O:
//             return P1

//         if P1 = invert(P2):
//             return O

//         x1, y1 := P1
//         x2, y2 := P2

//         if P1 = P2:
//             m := (3*x1^2 + a) / 2*y1
//         else:
//             m := (y2 - y1) / (x2 - x1)

//         x3 := m^2 - x1 - x2
//         y3 := m*(x1 - x3) - y1

//         return (x3, y3)

// The first three checks are simple - they pretty much just implement
// the rules we have for the identity and inversion.

// After that we, uh, use math. You can read more about that part
// elsewhere, if you're interested. It's not too important to us, but it
// (sort of) makes sense in the context of those graphs I'm not showing
// you.

// There's one more thing we need. In the multiplicative integers, we
// expressed repeated multiplication as exponentiation, e.g.:

//     y * y * y * y * y = y^5

// We implemented this using a modexp function that walked the bits of
// the exponent with a square-and-multiply inner loop.

// On elliptic curves, we'll use scalar multiplication to express
// repeated addition, e.g.:

//     P + P + P + P + P = 5*P

// Don't be confused by the shared notation: scalar multiplication is not
// analogous to multiplication in the integers. It's analogous to
// exponentiation.

// Your scalarmult function will look pretty much exactly the same as
// your modexp function, except with the primitives swapped out.

// Actually, you wanna hear something great? You could define a generic
// scale function parameterized over a group that works as a drop-in
// implementation for both. Like this:

//     function scale(x, k):
//         result := identity
//         while k > 0:
//             if odd(k):
//                 result := combine(result, x)
//             x := combine(x, x)
//             k := k >> 1
//         return result

// The combine function would delegate to modular multiplication or
// elliptic curve point depending on the group. It's kind of like the
// definition of a group constitutes a kind of interface, and we have
// these two different implementations we can swap out freely.

// To extend this metaphor, here's a generic Diffie-Hellman:

//     function generate_keypair():
//         secret := random(1, baseorder)
//         public := scale(base, secret)
//         return (secret, public)

//     function compute_secret(peer_public, self_secret):
//         return scale(peer_public, self_secret)

// Simplicity itself! The base and baseorder attributes map to g and q in
// the multiplicative integer setting. It's pretty much the same on a
// curve: we'll have a base point G and its order n such that:

//     n*G = O

// The fact that these two settings share so many similarities (and can
// even share a naive implementation) is great news. It means we already
// have a lot of the tools we need to reason about (and attack) elliptic
// curves!

// Let's put this newfound knowledge into action. Implement a set of
// functions up to and including elliptic curve scalar
// multiplication. (Remember that all computations are in GF(p), i.e. mod
// p.) You can use this curve:

//     y^2 = x^3 - 95051*x + 11279326

// Over GF(233970423115425145524320034830162017933). Use this base point:

//     (182, 85518893674295321206118380980485522083)

// It has order 29246302889428143187362802287225875743.

// Oh yeah, order. Finding the order of an elliptic curve group turns out
// to be a bit tricky, so just trust me when I tell you this one has
// order 233970423115425145498902418297807005944. That factors to 2^3 *
// 29246302889428143187362802287225875743.

// Note: it's totally possible to pick an elliptic curve group whose
// order is just a straight-up prime number. This would mean that every
// point on the curve (except the identity) would have the same order,
// since the group order would have no other divisors. The NIST P-curves
// are like this.

// Our curve has almost-prime order. There's just that small cofactor of
// 2^3, which is beneficial for reasons we'll cover later. Don't worry
// about it for now.

// If your implementation works correctly, it should be easy to verify:
// remember that multiplying the base point by its order should yield the
// group identity.

// Implement ECDH and verify that you can do a handshake correctly. In
// this case, Alice and Bob's secrets will be scalars modulo the base
// point order and their public elements will be points. If you
// implemented the primitives correctly, everything should "just work".

// Next, reconfigure your protocol from #57 to use it.

// Can we apply the subgroup-confinement attacks from #57 in this
// setting? At first blush, it seems like it will be pretty difficult,
// since the cofactor is so small. We can recover, like, three bits by
// sending a point with order 8, but that's about it. There just aren't
// enough small-order points on the curve.

// How about not on the curve?

// Wait, what? Yeah, points *not* on the curve. Look closer at our
// combine function. Notice anything missing? The b parameter of the
// curve is not accounted for anywhere. This is because we have four
// inputs to the calculation: the curve parameters (a, b) and the point
// coordinates (x, y). Given any three, you can calculate the fourth. In
// other words, we don't need b because b is already baked into every
// valid (x, y) pair.

// There's a dangerous assumption there: namely, that the peer will
// submit a valid (x, y) pair. If Eve can submit an invalid pair, that
// really opens up her play: now she can pick points from any curve that
// differs only in its b parameter. All she has to do is find some curves
// with small subgroups and cherry-pick a few points of small
// order. Alice will unwittingly compute the shared secret on the wrong
// curve and leak a few bits of her private key in the process.

// How do we find suitable curves? Well, remember that I mentioned
// counting points on elliptic curves is tricky. If you're very brave,
// you can implement Schoof-Elkies-Atkins. Or you can use a computer
// algebra system like SageMath. Or you can just use these curves I
// generated for you:

//     y^2 = x^3 - 95051*x + 210
//     y^2 = x^3 - 95051*x + 504
//     y^2 = x^3 - 95051*x + 727

// They have orders:

//     233970423115425145550826547352470124412
//     233970423115425145544350131142039591210
//     233970423115425145545378039958152057148

// They should have a fair few small factors between them. So: find some
// points of small order and send them to Alice. You can use the same
// trick from before to find points of some prime order r. Suppose the
// group has order q. Pick some random point and multiply by q/r. If you
// land on the identity, start over.

// It might not be immediately obvious how to choose random points, but
// you can just pick an x and calculate y. This will require you to
// implement a modular square root algorithm; use Tonelli-Shanks, it's
// pretty straightforward.

// Implement the key-recovery attack from #57 using small-order points
// from invalid curves.
