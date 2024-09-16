Troubleshooting 
=================

GLIBCXX 
---------

If when importing the `atlas` module into your python script you get an error stating 
``/lib/libstdc++.so.6: version GLIBCXX_3.4.32 not found``, it means that the library was 
built against a ``GLIBCXX`` version that is not listed in your runtime system's ``libstdc++``. 

If the import is trying to use the miniconda's ``libstdc++``, the problem can be solved by 
running either one of these two commands:

.. prompt:: bash $
    
    conda install -c conda-forge gcc=X.X

.. prompt:: bash $

    conda install -c conda-forge libstdcxx-ng=Y

where `X` and `Y` are the package versions that ensure the desired ``GLIBCXX`` version is 
available. Another solution involves linking with the system library via: 

.. prompt:: bash $

    ln -sf /usr/lib/x86_64-linux-gnu/libstdc++.so.6 ${CONDA_PREFIX}/lib/libstdc++.so.6
