# WSF (IT, XM, S3M, MOD Sample Packer)

Written, and released into the public domain by Josh Whelchel (@soundofjw)[http://www.twitter.com/soundofjw]

# What is it?

WSF is a format designed to reduce redundancy in modules. In a collection of modules that uses the same samples (game soundtracks),
a lot of data for these samples will be contained twice. Obviously, this is not ideal.

WSF stores samples in a separate sample pack container (the WSP) - any duplicate samples (including samples derived from others) will dupe-checked and stored appropriately.

A side benefit: WSF also allows music to be protected using a very primitive password-encoding scheme, to encourage those who enjoy music to seek it through more appropriate channels.

# Who has used it?

WSF has been used in Konjak's Noitu Love 2, The Spirit Engine 2, Kyle Pulver's Bonesaw: The Game, and several other products.