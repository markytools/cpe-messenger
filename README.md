<p><strong><u>Description:</u></strong></p>
<p>CPE Messenger is a lightweight social media application template that defines a simple social media app. The platform for this involves both a desktop and an android version. It uses Backendless as a third-party cloud server to store the database of the social media account. It consists of the following capabilities:</p>
<ul>
<li>Friend/Unfriending a person (called buddies)</li>
<li>Group Chat</li>
<li>Personal storage of files</li>
<li>Jackpot, an interactive simple gamble game</li>
</ul>
<p>&nbsp;</p>
<p><strong><u>Requirements:</u></strong></p>
<ul>
<li>Version should be &gt;= Qt 5.9.1</li>
<li>Use &gt;= Java 8 (should be 32-bit)</li>
<li>Use &gt;= Android NDK r10e (should be 32-bit): https://developer.android.com/ndk/downloads/older_releases</li>
<li>Updated version of Android SDK, however use the old version of the &ldquo;tools&rdquo; folder (Qt issue here: <a href="https://stackoverflow.com/questions/42754878/qt-creator-wont-list-any-available-android-build-sdks">https://stackoverflow.com/questions/42754878/qt-creator-wont-list-any-available-android-build-sdks</a>). You can download the old tools folder of the SDK here: https://dl.google.com/android/repository/tools_r25.2.5-windows.zip, then make sure to replace the original tools folder with this.</li>
</ul>
<p>(For easy setup, use the recommended lib versions above instead of older or newer versions)</p>
<p>Setup all of these in the Qt &gt;&gt; Tools &gt;&gt; Options &gt;&gt; Devices.</p>
<p>&nbsp;</p>
<p>Since this application makes use of the Qt Network Library, you need to copy the following DLL file onto the mingw build path where the .exe of your project resides</p>
<p>C:\Qt\Tools\mingw530_32\opt\bin\libeay32.dll</p>
<p>C:\Qt\Tools\mingw530_32\opt\bin\ssleay32.dll</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>For the Backendless network, I&rsquo;ve used my own Backendless account, along with my APPLICATION ID and REST KEY. So, these keys are already provided in the code, but if you want to use your own Backendless account, the following steps are needed to be done:</p>
<p><img src="https://raw.githubusercontent.com/markytools/cpe-messenger/master/other/requirementImg/1.png" alt="" width="239" height="236" />Make sure that these are the tables present in Backendless</p>
<p>&nbsp;</p>
<p><img src="https://raw.githubusercontent.com/markytools/cpe-messenger/master/other/requirementImg/2.png" alt="" width="338" height="291" />The &ldquo;Buddies&rdquo; Table schema</p>
<p>&nbsp;</p>
<p>&nbsp;<img src="https://raw.githubusercontent.com/markytools/cpe-messenger/master/other/requirementImg/3.png" alt="" width="327" height="298" />The &ldquo;CashData&rdquo; Table schema</p>
<p>&nbsp;</p>
<p><img src="https://raw.githubusercontent.com/markytools/cpe-messenger/master/other/requirementImg/4.png" alt="" width="319" height="181" />&rdquo;Files&rdquo; schema</p>
<p>&nbsp;</p>
<p><img src="https://raw.githubusercontent.com/markytools/cpe-messenger/master/other/requirementImg/5.png" alt="" width="282" height="211" />&rdquo;Groups&rdquo; schema</p>
<p>&nbsp;</p>
<p><img src="https://raw.githubusercontent.com/markytools/cpe-messenger/master/other/requirementImg/6.png" alt="" width="285" height="154" />&rdquo;ProfileNames&rdquo; schema</p>
<p>&nbsp;</p>
<p><img src="https://raw.githubusercontent.com/markytools/cpe-messenger/master/other/requirementImg/7.png" alt="" width="269" height="347" />&rdquo;Users&rdquo; schema</p>
<p>&nbsp;</p>
<p>Also, add just one data in the &ldquo;ProfileNames&rdquo; table. Set the profNames data to [] and remember the objectId of this data.</p>
<p><img src="https://raw.githubusercontent.com/markytools/cpe-messenger/master/other/requirementImg/8.png" alt="" width="624" height="96" /></p>
<p>&nbsp;</p>
<p>In the settings.cpp file of the program, set the Settings::PROFNAMES_OBJ_ID with this objectId. Likewise, replace the Settings::APPLICATION_ID and Settings::REST_API_KEY with your account&rsquo;s Application ID and Rest API key.</p>
