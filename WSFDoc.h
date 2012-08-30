/// WSF Doc

/// \file WSFDoc.h
/// WSF Document
/// This supplies information for DOXYGEN.

/*!	\mainpage Whelchel Sound Format

	<center>

	<b><h1>WSF Format</h1></b><br>
	</center>

	\section news WSF Module Packer Documentation
 
    <small>Please apologize any crude remarks in the documentation and code. This code was written in 2003 when I was 16 years old. :P<br/>
        You can find me on <a href="http://www.twitter.com/soundofjw">twitter</a> and harrass me about it. - Josh, 2012</small>

	<table width=100% border=0>
	<tr>
	<td width=65% valign=top>

	<b><h2>Important Links</h2></b>\n\n

	\ref wsffaq "WSF FAQ" \n
	\ref wsfohc \n

	\ref wsfapplies "Why WSF" \n
	\ref usingwsfpage \n
	\ref filedocpage "File Documentation" \n

	\ref wsfdev \n

	\ref wsfsource \n
	\ref todo \n
	<a href="bug.html" class="el">Bug List</a> \n

	\ref credits\n

	</td>
	<td valign=top width=35% valign=top>

	<b><h3>Latest Versions</h3></b>\n\n

	<b>WSFTool</b>: #WSFTVERHI\n
  	<b>WSFPlay</b>: #WPLVERHI\n
	<b>INWSF</b>: #INWVERHI\n
	<b>WSFDLL</b>: #WSFD_CURVERHI\n
	\n
	<b>WSFMod</b>: #WSFMVERHI\n
	<b>WSFPack</b>: #WSFPVERHI\n
	\n\n
	<b><small>Supported Formats:</small></b>\n
	IT, S3M, XM, MOD (WSF, WTF, WGM)\n(OGG Compression!)\n\n

	<b>Confused About the Versions?</b>\n
	<small>You can tell the version by clicking the appropriate link above
	and adding the two numbers you see. You should see a VERHI, and a VERLO.
	Assume the version is "Version VERHI.VERLO"</small>

	</td>
	</tr>
	</table>

	*/

/*!

	\page usingwsfpage How to Use

	\section makewsf Making & Managing WSFs

	Making WSFs is rather simple. You can see what formats WSFTool currently
	supports by using the flag -showext.\n

	You will need \ref wsftoolssec "WSFTool" in order to create WSFs\n

	\subsection howtowsf How to Create WSF files

	After you have downloaded WSFTool you can easily begin to create
	WSF files from your mods.

	-# <b>Sort your modules</b>\n
	Place all your modules in one directory
	and have them named how you like.\n

	-# <b>Run WSFTool</b>\n
	In the command line, go to the directory of your modules.
	If the directory of your WSF utils is not in the path,
	you should put it there.\n
	\code WSFTool *.it -MThePack.wsp \endcode \n
	\note ThePack.wsp may be replaced with any filename.\n
	You can add to this pack by running the same thing with a diffrent wildcard.\n
	(Sample packs may hold more than one type of file format!)\n\n

	
	-# <b>Distribute your WSFs</b>\n
	In order to distribute your WSFs, the WSP file 
	(or whatever file you generated as the pack) 
	must be included with it.\n

	\subsection howtotag How to Tag WSF Files

	Tagging WSF files is rather simple. Tags are simply ascii fields within a WSF for
	commenting and display use. For instance, the title of a track is a tag, and so is the artist.
	Other possibilities include Game, Comments, and Editor etc.\n

	To Tag WSFs you run the following:\n
	\code WSFTool *.wsf -T -tGame=Jazz_JackRabbit -tArtist=Epic_MegaGames \endcode
	\note -T and -t are two diffrent options, -t may be used in creating WSFs as well (-M)\n
	Use an underscore (_) to create spaces ' '.

	The preceding code sets tags on ALL the WSFs in the directory.
	The 'Game' tag is set to "Jazz JackRabiit", and 'Artist' to "Epic MegaGames",
	and so forth.

	You may be more familiar with tags if you've used the INWSF configuration screen.
	These are where the %title% and %artist% etc get their values. (The %tag% parsing
	is done by CWSFAscii::MakeTitle)

	\subsection passprotect	Protecting your Modules

	If for some reason you want to protect the modules so that other people cannot
	listen to them, then you may use the password encoding system. This is VERY handy
	for games, seeing as many users will want to hear a sountrack with all the ease.

	To create WSFs with a passworded pack, you use the -x option on WSF.
	\code WSFTool *.it -MMyPak.wsp -xMyPassword \endcode

	<b>IF YOU ADD MORE SONGS TO THE PACK, THEY MUST SHARE THE PASSWORD!</b>

	\subsection howtorestore Restoring the Original Modules

	Because WSF is <b>LOSELESS</b>, all WSFs can be restored back into their original formats.\n
	To do this with WSFTool, you run the following:\n
	\code WSFTool *.wsf -W \endcode
	
	This will convert all wsfs in the directory back into their original file type. The files
	created are xxxx.wsf -> xxxx.yyy (yyy being the module type)\n

	However, for this to work, the module format must be supported by your current version
	of WSFTool. Due to the opensource nature of this project, a developer could easily develop
	his or her own loader for WSF and not contribute the loader source for WSFTool.

	\section Hearing WSF

	Currently, WSF files are made mainly to be played within a host.
	However, because of \ref wsfuses "WSFs many uses", players
	will be requested. Currently, the best way to listen to WSFs
	for leisure is \ref INWSF "IN_WSF", the <a href="http://www.winamp.com">Winamp2</a> Plugin.\n
	Also avalible is \ref WSFPlay "WSFPlay", a standalone player.\n\n

	Supported IT Based Players
	- \ref INWSF "IN_WSF"
	- \ref WSFPlay "WSFPlay"

	All of the mentioned players can be found on the \ref wsfdownls "WSF Downloads Page".

	\section useincode Using WSF in your Code

	For examples of using WSF in your code see \ref wsfincode "this page"


  */

/*!
	\page wsfapplies Applications of WSF

	\section wsfuses Common Uses of WSF

	\li <b>One Hour Competitions</b>
	\par 
	WSFs make a great use for OHCs and other compos that rely on sample packs.
	Seeing as samples are duplicate across all the mods entered in a ohc, much space
	can be saved on the release of a votepack. The host could also request that every
	entry be 'WSF'd so and sent in without the accompanying sample pack (which would be
	packaged with the votepack)


	\li <b>Games</b>
	\par 
	WSFs are also GREAT for game distribution. If a musician decided to use the
	same samples throughout a soundtrack, he could WSF them to save a great deal of
	space.


	\li <b>Album Distribution</b>
	\par 
	Seeing as many module albums use duplicate samples, WSF is an obvious space
	saver, and with its built in Ascii Tag functions (CWSFAscii::GetWSFTags, CWSFAscii::AddWSFTags),
	storing titles, album names, etc, couldn't be easier.
	
	
  */

/*!
	\page wsffaq FAQ (Frequently Asked Questions)

	- <b>What is WSF?</b>\n
		WSF is a format that packs a group of audio files (such as MODs) into
		a group of WSF files and a samplepack. This prevents the use of duplicate
		samples throughout a large library of music, such as a game soundtrack.

	- <b>How can I start testing WSF</b>?
		-# \ref wsfplayersec "Download" WSFPlay or INWSF if you have Winamp 2
		-# \ref wsfsamplessec "Download" one of the sample WSF packs
		-# Load one of the WSF's into Winamp of WSFPlay and start headbanging
		-# If you experiance any troubles, contact <a href="mailto:joshwhelchel@hotmail.com">Josh Whelchel</a>

	- <b>Will there be an MMF Extension?</b>\n
	Actually, two! You can obtain both of these, ModFusion eX & ModFusion from either <a href="http://www.syntesis.org">Syntesis</a> or
	<a href="http://www.acsv.net">Apocalyptic Coders</a>. However, it should be noted that
	ModFusion 1 (not eX) will only support WSF's made in compliance with the ModFusion 1 Specifications
	described in \ref wsf.txt \n\n

	- <b>Enter Password?</b>\n
		If you're using WSFPlay or INWSF (and maybe other players have the feature) then you may be asked for
		a password when playing a WSF. Some WSF files are password protected so that not everyone can listen
		to them. If you enter an incorrect password, you will still be able to hear the song, but you will
		notice glitches and undesireable clicks in the playback. For more information on this see \ref passprotect

	- <b>Can I change filenames to include tag information?</b>\n
		You may have many WSF/WTFs which have weird filenames, like SR_45.wtf etc.
		If you're a neat freak, you're going to want to name the wtfs so you can
		access the files quicker. To do this, all you have to do is use
		WSFTool's -N option.\n
		\code WSFTool *.wtf "-NSpanorak - %title%.wtf" \endcode \n

		This works just like INWSF's set title format option!

	- <b>Is WSF opensource?</b>\n
	\ref wsfsource Yes

*/

/*!
	\page wsfsource WSF Source

	\section wsfsrcsec Whelchel Sound Format Source

	Yes, the WSF format <i>is</i> opensource.
	For exmaples see \ref wsfincode

	\subsection getsrc Obtaining the Source

	<a href="http://www.github.com/soundofjw/WSF">github.com</a>

	\subsection aboutsrc About the Source

	The WSF mainframe is a portable set of classes, consiting of
	CWSFMod and CWSFPack along with loading classes derived CBaseLoader.
	One such class is CS3MLoader.\n

	The main files you'll need for using the WSF format are
	
	  - WSFGlob.h\n
	  Global Definitions

	  - WSFMod.cpp + WSFMod.h\n
	  Base MOD Handling Functions + CWSFAscii

	  - WSFPack.cpp + WSFPack.h\n
	  WSFPack Handling Functions (CWSFPack)

	  - WSFLoaders.cpp\n
	  WSFLoaders : CITLoader + CS3MLoader + CWSFLoader + others as added

	  - WSFComp.c + WSFComp.h\n
	  Compression Handling for CWSFPack and CWSFLoader
	 
	Any loaders you create for yourself may go in a seperate file that includes
	WSFMod.h and must be a child of CBaseLoader\n

	\subsection includesrc Including WSF in your project

	To include WSF in your project, you will simply need to compile and link
	the above *.c* (.cpp + .c) files with your project, and include the requested
	.h files. If you don't need one of the above features, simply do not include
	it in your linking (.c).\n

	An alternative is \ref wsflibpage \n

	\section wsfloaders Creating a WSF Loader

	This section explains how to create your own file loader for WSF.\n\n

	<OL>
	<LI> <b>Add to the WSFModVers enum</b>\n
	First, in the #WSFModVers enum (in WSFMod.h), create an entry that suits
	the file format your using best. (eg WSFM_xxx -> WSFM_XM)\n
	</LI>

	<LI> <b>Add Description and Extension</b>\n
	Go to the definition of #g_cModExt (WSFMod.cpp) and add an entry (which
	is the same placement of your enum entry before) that is the extension
	of your format. Use the format ".XXX", and do not exclude the '.'\n
	In the definition of #g_cModDesc do the same as above, but instead create
	a suitable description for your format. Such as, "Impulse Tracker Module"\n
	In the definition of #g_bModFlag you need to add an entry with the correct
	placement that suits whether or not your loader is enabled. 1 = TRUE, 0 = FALSE\n
	</LI>

	<LI> <b>Create a new Loader Class</b>\n
	In WSFMod.h, after the definition of CBaseLoader, you need to create child
	class of it. Name it something like CxxxLoader, where xxx is the file extension.
	For instance, the S3M loader is named CS3MLoader.\n
	\code
	class CxxxLoader : public CBaseLoader
	{
	public:

		// Base Function for Loading
		int Load( wsfb *bData, wsul nSize, wsf_loaddata *wLD );

		// Base Saving Function
		int Save( wsf_modout *wOut, wsf_loaddata *wLD );

		// Function to get pointers to ACTUAL samples
		int GetSamplePointers( wsfb *bData, wsul nSize, wsf_gspdata *gspData );

	};
	\endcode
	The three functions are REQUIRED functions that must be inherited.\n
	</LI>

	<LI> <b>Code the Functions</b>\n
	In WSFLoaders.cpp, or perferably in your own source file, start
	creating the code. All the functions should return ZERO for success,
	and NONZERO for failure. The MACROs at the end of WSFMod.h should be
	used for reading and writing. These are:\n
	#MOPEN, #MREAD, #MAPP, #MRESET, #MSEEK, #MBEGIN, #MEND, #MBACK\n

	<UL>
	<LI> <b>GetSamplePointers</b>\n
	This function needs to fill a wsf_gspdata structure with the following information:\n

	<UL start="10">
	<LI> <U>Number of Samples (nSamples)</U>\n
	Fill this in with the number of samples.\n
	</LI>

	<LI> <U>Sample Offsets (nSampOffs)</U>\n
	Array of wsul that contains the offsets to the ACTUAL sample data within
	the file. It should be offset of 0.\n
	</LI>

	<LI> <U>Sample Sizes (nSampSizes)</U>\n
	Array of wsul that contain the sizes of each of the samples in bytes.\n
	</LI>

	<LI> <U>Channels, Bits, and InnerSamples (nCh, nBit, nInSamps)</U>\n
	Array of bytes (nInSamps is wsul) that contain the number of channels,
	bit rate (8 or 16), and number of samples of each sample.\n
	If the number of samples isn't stored, you can generally retrieve it like this:\n
	\code
	nInSamps[i] = nSampSizes[i] / nCh[i] / (nBit[i]/8);
	\endcode
	\n			
	</LI>

	</UL>

	This function should also be the one to make sure the loader is opening
	the correct file format.\n
	A good example of this code is found at CS3MLoader::GetSamplePointers (WSFLoaders.cpp)\n
	
	\note After you have set the number of samples, you can easily init
	the memory in the wsf_gspdata structure using InitGSPData().
	However, you should not free this on your own.\n
	
	\note If you perfer to simply add to the number of samples as you go along,
	then you must run InitGSPData with gspData->nSamples = 0 and use the function
	PushIntoGSP().\n

	\n
	</LI>

	<LI> <b>Load</b>\n
	This function is mainly responsible for loading the samples into a wsf_sample array,
	their sizes into a wsul array, and storing the MOD data after the samples have been
	loaded.\n
	This is done by completeing a wsf_loaddata structure. Alot of this however, is very
	easy considering the GetSamplePointers function should be called to do this.

	<UL start="10">
	<LI> <U>Number of Samples (nSamples)</U>\n
	Load this from gspData->nSamples\n
	</LI>

	<LI> <U>Samples & Sizes (bSamples & nSampleSizes)</U>\n
	bSamples is an array of wsf_sample which must be filled
	out for every sample in the file. <b>If the sample size is
	0, then DO NOT allocate bSamples[nSamp].bModData!!!</b>\n
	This data may also be filled out easily from the arrays in wsf_gspdata.
	</LI>

	<LI> <U>Mod Data (bModData + nModSize)</U>\n
	After the samples are loaded, you can easily fill this out with the following code:\n
	\code
	// Load MOD data
	wLD->bModData = (wsfb*)malloc(nSize);
	wLD->nModSize = nSize;
	memcpy(wLD->bModData,bData,nSize);
	\endcode
	\n
	</LI>
	
	<LI> <U>Other Fields (nSubDiff, cTitle, ...)</U>\n
	Most of these are self-explanatory, except nSubDiff and nModType.
	You should leave nModType alone seeing as it should already be
	setup, but in case you want to make sure you can set it to the
	value of the loader's entry in the WSFModVers enum. nSubDiff should
	be set to 0.\n
	</LI>

	</UL>
	
	Loading is one of the trickier functions. After you call GetSamplePointers,
	be sure to call FreeGSPData to free the pointer. An example of calling
	this function shows:\n
	\code
	wsf_gspdata gD;
	if (GetSamplePointers(bData,nSize,&gD)){
		// Error
		return;
	}
	FreeGSPData(&gD);
	\endcode
	\n
	</LI>

	<LI> <b>Save</b>\n
	This function is responsible for saving the data in a wsf_loaddata structure
	into a memory buffer. Normally, this function does nothing, and the samples
	are replaced automatically through CWSFMod::ReplaceSamplesForSave().
	<b>However</b>, should this function return 1, the entire saving process
	can be performed in this function. If it returns a value higher than 1,
	than an error is assumed. You can return 0 and modify the sample data if
	you need to change the sample data before saving the file (As in CXMLoader::Save())\n
	</LI>

	</UL>

	</LI>

	<LI> <b>Add Loading Code</b>\n
	Go to GetBaseLoader() in WSFMod.cpp and add a case to create a new of your class.\n
	If you made CMODLoader for #WSFM_MOD, you will do the following:\n
	\code
	case WSFM_MOD:
		xBL = (CBaseLoader*) new CMODLoader;
		break;
	\endcode
	\n
	</LI>

	<LI> <b>Add Playing Code</b>\n
	You can add playing code in WSFPlay.cpp where you see the switch (bType) line.
	You will regocnize all your favorite WSFModVers enum entries, and you should
	add your player code in just as it does. If you're format isn't support by
	the DUMB sound module, which it most likely isn't, then you'll need to change
	the callback thread and handling after the switch for your specific format.\n
	</LI>

	</OL>

	Your WSF Loader should now be complete! Make sure to study the CS3MLoader, as it
	is the simplest loader.

	\section wsfincode WSF Code Usage

	This section contains examples and codesnippets.

	\subsection general Player Example

	Two examples of WSF players are included.\n
	WSFPlay.cpp and in_wsf.cpp (Winamp Plugin)

	\subsection loading Loading WSFs into their original format

	\code

	char *cPass;
	wsf_modout g_wO;
	wsfb bType;
	CWSFMod *g_wMod;
	g_wMod = new CWSFMod;

	// If user gave a password, set it here
	if (cPass != NULL)
		g_wMod->SetPassSum(GenerateSum(cPass,strlen(cPass));		

	// Load the WSF
	if (g_wMod->LoadMod(fn))
		return 1;

	// Check to see if there was a password and if we didn't get one
	if (g_wMod->GetPack()->m_nNeedPass && cPass == NULL)
	{
		// Notify User
		printf("ENTER A PASS NEXT TIME?!");
	}

	// Get Type
	bType = GetModType();

	// Load Extract a MOD (nonWSF)
	if (g_wMod->WriteMod(&g_wO,FALSE))
		return 1;

	// Using the bType (which is filled w/ a value from the WSFModVers enum)
	// LOAD THE MOD into the modplayer and start the callback thread
	// (I reccomend switch (bType) { case WSFM_IT: LoadIT(); break; case WSFM_S3M: LoadS3M(); break; } etc)
	// You load using g_wO.bModData and g_wO.nSize

	// Once the module is loaded into its player's memory, you may free using the code below.
	// Its reccomended to do this before the music actually starts playing.
	// So you could call this directly after your Load() function.

	// Free the mod
	g_wMod->FreeMod();

	// The following line should be omitted if more music will be played from the SAME sample pack
	g_wMod->FreePack();

	// Free the mod data
	free(g_wO.bModData);
	g_wO.nSize = 0;

	delete g_wMod;
	\endcode

	\note Its important that you note the g_wMod->FreePack(); line.
	You shouldn't need it when creating a game that will use the same sampleset.

*/

/*!
	\page wsflibpage WSFLib

	This documentation is not complete ):\n
	Making this library should make wrapping CWSFxxx easier.

*/

/*!

	\page wsfohc WSF: An OHC Revelation!

	\section wsfohcrev WSF: A One Hour Compo Revelation

	Through the use of MiniWSF and MeltWSF, one hour compo votepacks can
	become amazingly smaller! How? Read this following guide:

	- <b>What is WSF/WTF?</b>\n
	
	WSF (Whelchel Sound Format) and WTF (Whelchel Tune Format) are
	two different names for the same thing. The WTF extension was
	adopted after the conflict with Windoze Scripting Format was
	identified. <b>WSF</b> is a simply a means of compressing a group
	of modules with duplicate or simply modified samples (reversed, appended, cut, etc).
	
	- <b>What does this mean for an OHC?</b>\n

	Well, for those who don't know, One Hour Compos (OHCs) usually
	involve Sample Packs. The entriants in an OHC are limited to using
	these samples. This means that these samples will be used many times
	throughout many songs when it comes to voting time. There are many
	compos based on OHC format as well, including Bar Wars. <b>WSF</b> will
	take these modules, and remove the samples from all of them and store them
	in a WSP file. This acts as the 'Sound Bank' for our tracks. <b>Put Simply:</b>
	Voters will be downloading each sample once, instead of once for every track.

	\subsection wsfohcstart Getting Started
	
	So you probably want to get started using WSF. There are a number of ways:

	- <b>OHC Hosts on a Windows system</b>\n
		OHC Hosts on Windows simply need to download \ref wsftoolssec "MiniWSF" from the
		download page. They run the executable, and will select the modules that were
		entered. A self extracting windows executable will then be built (a 68kb basefile!)
		from these modules, and you'll see a signifigant size decrease. YAY. <b>But what about
		the voters that aren't on windows?!</b> A good question. MeltWSF source should
		be included in the votepack for *nix users. (MeltWSF is gcc compliant) 
		The source is found on the downloads page in it's own small package 
		(opposed to the full WSF Source).

	- <b>OHC Voters not on Windows</b>\n
		OHC Voters should obtain the MeltWSF source (MeltWSF is gcc compliant). MeltWSF can take a MiniWSF windows
		executable (without actually executing it) and extract the music. Very simple.

	- <b>OHC Hosts <i>NOT</i> using Windows</b>\n
		OHC Hosts that aren't on Windows should obtain the MeltWSF source and compile it.
		(MeltWSF is gcc compliant). To build a votepack (<i>that requires MeltWSF to extract</i>)
		they run MeltWSF like so:\n
		\code MeltWSF build myvotepackfile.mwp "my ohc title" "my message" file1 file2 file3 \endcode \n

	- <b>OHC Voters voting with a nonexecutable pack</b>\n
		-# Obtain MeltWSF (either the source, or the windows binary) from the downloads section.\n
		-# Run <b>make -f meltmake</b> to build it (if you have the source)\n
		-# Run MeltWSF like so: meltwsf mypack.mwp\n
	\n

	<b>ENJOY!</b>

*/


/*!
	\page filedocpage WSF File Documentation

	\verbinclude wsf.txt
*/

/*! \page wsfdev Development Log

	\verbinclude WSFLog.txt

*/

/*!
	\page credits Credits & Thanks

	<b>WSF</b> would not be without the following people:

	- <b>Josh Whelchel</b>\n
	<a href="http://www.twitter.com/soundofjw">Me.</a> I wrote this when I was 16, and figured it should be otu in the open.

	- <b>Simon Pittock</b>\n
	Helped with the Sample Relationship therom found in CWSFPack::CreateSignature

	- <b>Nobuyuki</b>\n
	Convinced me to add all the features that I thought were stupid... :< <a href="http://nyuu.cellosoft.com">Nyuu</a>\n
	Big help with stuff like sample previewing and sample crediting.

	- <b>Stephen Rhodes</b>\n
	Designed the WSF logo!

	- <b>DUMB Module Library</b>\n
	WSFPlay and INWSF use <a href="http://dumb.sourceforge.net/">DUMB</a> to render mods!

	- <b>Apocalyptic Coders</b>\n
	For all their patience with me <b>;_;</b>
*/
