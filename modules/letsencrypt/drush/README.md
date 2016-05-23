LETSENCRYPT
===========

Maintenance
-----------

We include the letsencrypt.sh script from https://github.com/lukas2511/letsencrypt.sh directly, to avoid issues with packaging scripts on drupal.org. We should strive to only use released versions of this script. Either way, we should mention the tag (or, if absolutely required, the commit hash) in the commit message when updating the script.

Basically, this should look something like:

    $ cd /path/to/this/module/bin/
    $ wget https://raw.githubusercontent.com/lukas2511/letsencrypt.sh/*COMMIT_TAG*/letsencrypt.sh
    $ git diff   # Ensure we're making an atomic commit
    $ git commit -am"Update letsencrypt.sh to 0.2.0."

