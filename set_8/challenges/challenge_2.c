
// // ------------------------------------------------------------

// 58. Pollard's Method for Catching Kangaroos

// The last problem was a little contrived. It only worked because I
// helpfully foisted those broken group parameters on Alice and
// Bob. While real-world groups may include some small subgroups, it's
// improbable to find this many in a randomly generated group.

// So what if we can only recover some fraction of the Bob's secret key?
// It feels like there should be some way to use that knowledge to
// recover the rest. And there is: Pollard's kangaroo algorithm.

// This is a generic attack for computing a discrete logarithm (or
// "index") known to lie within a certain contiguous range [a, b]. It has
// a work factor approximately the square root of the size of the range.

// The basic strategy is to try to find a collision between two
// pseudorandom sequences of elements. One will start from an element of
// known index, and the other will start from the element y whose index
// we want to find.

// It's important to understand how these sequences are
// generated. Basically, we just define some function f mapping group
// elements (like the generator g, or a public key y) to scalars (a
// secret exponent, like x), i.e.:

//     f(y) = <some x>

// Don't worry about how f is implemented for now. Just know that it's a
// function mapping where we are (some y) to the next jump we're going to
// take (some x). And it's deterministic: for a given y, it should always
// return the same x.

// Then we do a loop like this:

//     y := y * g^f(y)

// The key thing here is that the next step we take is a function whose
// sole input is the current element. This means that if our two
// sequences ever happen to visit the same element y, they'll proceed in
// lockstep from there.

// Okay, let's get a bit more specific. I mentioned we're going to
// generate two sequences this way. The first is our control
// sequence. This is the tame kangaroo in Pollard's example. We do
// something like this:

//     xT := 0
//     yT := g^b

//     for i in 1..N:
//         xT := xT + f(yT)
//         yT := yT * g^f(yT)

// Recall that b is the upper bound on the index of y. So we're starting
// the tame kangaroo's run at the very end of that range. Then we just
// take N leaps and accumulate our total distance traveled in xT. At the
// end of the loop, yT = g^(b + xT). This will be important later.

// Note that this algorithm doesn't require us to build a big look-up
// table a la Shanks' baby-step giant-step, so its space complexity is
// constant. That's kinda neat.

// Now: let's catch that wild kangaroo. We'll do a similar loop, this
// time starting from y. Our hope is that at some point we'll collide
// with the tame kangaroo's path. If we do, we'll eventually end up at
// the same place. So on each iteration, we'll check if we're there.

//     xW := 0
//     yW := y

//     while xW < b - a + xT:
//         xW := xW + f(yW)
//         yW := yW * g^f(yW)

//         if yW = yT:
//             return b + xT - xW

// Take a moment to puzzle out the loop condition. What that relation is
// checking is whether we've gone past yT and missed it. In other words,
// that we didn't collide. This is a probabilistic algorithm, so it's not
// guaranteed to work.

// Make sure also that you understand the return statement. If you think
// through how we came to the final values for yW and yT, it should be
// clear that this value is the index of the input y.

// There are a couple implementation details we've glossed over -
// specifically the function f and the iteration count N. I do something
// like this:

//     f(y) = 2^(y mod k)

// For some k, which you can play around with. Making k bigger will allow
// you to take bigger leaps in each loop iteration.

// N is then derived from f - take the mean of all possible outputs of f
// and multiply it by a small constant, e.g. 4. You can make the constant
// bigger to better your chances of finding a collision at the (obvious)
// cost of extra computation. The reason N needs to depend on f is that f
// governs the size of the jumps we can make. If the jumps are bigger, we
// need a bigger runway to land on, or else we risk leaping past it.

// Implement Pollard's kangaroo algorithm. Here are some (less
// accommodating) group parameters:

//     p = 11470374874925275658116663507232161402086650258453896274534991676898999262641581519101074740642369848233294239851519212341844337347119899874391456329785623
//     q = 335062023296420808191071248367701059461
//     j = 34233586850807404623475048381328686211071196701374230492615844865929237417097514638999377942356150481334217896204702
//     g = 622952335333961296978159266084741085889881358738459939978290179936063635566740258555167783009058567397963466103140082647486611657350811560630587013183357

// And here's a sample y:

//     y = 7760073848032689505395005705677365876654629189298052775754597607446617558600394076764814236081991643094239886772481052254010323780165093955236429914607119

// The index of y is in the range [0, 2^20]. Find it with the kangaroo
// algorithm.

// Wait, that's small enough to brute force. Here's one whose index is in
// [0, 2^40]:

//     y = 9388897478013399550694114614498790691034187453089355259602614074132918843899833277397448144245883225611726912025846772975325932794909655215329941809013733

// Find that one, too. It might take a couple minutes.

//     ~~ later ~~

// Enough about kangaroos, let's get back to Bob. Suppose we know Bob's
// secret key x = n mod r for some r < q. It's actually not totally
// obvious how to apply this algorithm to get the rest! Because we only
// have:

//     x = n mod r

// Which means:

//     x = n + m*r

// For some unknown m. This relation defines a set of values that are
// spread out at intervals of r, but Pollard's kangaroo requires a
// continuous range!

// Actually, this isn't a big deal. Because check it out - we can just
// apply the following transformations:

//     x = n + m*r
//     y = g^x = g^(n + m*r)
//     y = g^n * g^(m*r)
//     y' = y * g^-n = g^(m*r)
//     g' = g^r
//     y' = (g')^m

// Now simply search for the index m of y' to the base element g'. Notice
// that we have a rough bound for m: [0, (q-1)/r]. After you find m, you
// can plug it into your existing knowledge of x to recover the rest of
// the secret.

// Take the above group parameters and generate a key pair for Bob. Use
// your subgroup-confinement attack from the last problem to recover as
// much of Bob's secret as you can. You'll be able to get a good chunk of
// it, but not the whole thing. Then use the kangaroo algorithm to run
// down the remaining bits.
