# ---------------------------------------------------------
# ���O����(Windows)
# ---------------------------------------------------------
�EVisual Studio 2015


# ---------------------------------------------------------
# ���O����(Android NDK)
# ---------------------------------------------------------
�Endk-build
  AndroidNDK�̃C���X�g�[���p�X�����ϐ��ɐݒ肵�Ă���
  C:\Users\...\AppData\Local\Android\Sdk\ndk-bundle


# ---------------------------------------------------------
# zlib 1.2.8
#    https://zlib.net/
# ---------------------------------------------------------
1. zlib�\�[�X���_�E�����[�h���W�J���Ă���
   ��libpng�̃f�t�H���g��zlib1.2.8�ɂȂ��Ă��邽�߁A1.2.8���g�p����
   ��1.2.8�ȊO���g�p����ꍇ�́Alibpng�̃r���h�ݒ��M��K�v�����邩��

2. libpng�Ńr���h���邽�߉������Ȃ�
   
   [���C�u����]
   libpng�̃t�H���_
   [�w�b�_]
   zconf.h
   zlib.h


# ---------------------------------------------------------
# libpng 1.6.32
#    http://www.libpng.org/pub/png/libpng.html
# ---------------------------------------------------------
1. libpng�\�[�X���_�E�����[�h���W�J���Ă���
   ��zlib��W�J�����ꏊ�Ɠ����t�H���_

2. projects\vstudio\vstduio.sln���J��
   VisualStudio2015���g�p����ƁA�A�b�v�O���[�h���邩������邽�߁A�uOK�v�Ƃ���

   ��zlib1.2.8�ȊO�̃o�[�W�������g�p����ꍇ�́Aprojects\vstudio\zlib.props���C������H
     <ZLibSrcDir>..\..\..\..\zlib-1.2.8</ZLibSrcDir>

3. x64�r���h��L���ɂ���
   �\�����[�V������I�����u�v���p�e�B�v���u�\���v���p�e�B�v���u�\���}�l�[�W���[�v
   ���A�N�e�B�u�\�����[�V�����v���b�g�t�H�[���̃v���_�E������u�V�K�쐬�v
   ���V�����v���b�g�t�H�[����x64��I�����uOK�v

4. ReleseLibrary��Win32,x64���r���h����
   ���r���h�G���[���o�邪�Alibpng��zlib�̃��C�u�����͐������Ă���̂Ŗ���
   
   [���C�u����]
   projects\vstudio\
      - x64\Release Library\
           - libpng.lib
           - zlib.lib
      - Release Library\
           - libpng.lib
           - zlib.lib
   [�w�b�_]
   png.h
   pngconf.h
   pngdebug.h
   pnginfo.h
   pnglibconf.h
   pngpriv.h
   pngstruct.h


# ---------------------------------------------------------
# freetype 2.8.0
#    https://www.freetype.org/
# ---------------------------------------------------------
1. freetype�\�[�X���_�E�����[�h���W�J���Ă���

2. builds\windows\vc2010\freetype.sln���J��
   VisualStudio2015���g�p����ƁA�A�b�v�O���[�h���邩������邽�߁A�uOK�v�Ƃ���

3. Relese Multithreaded��Win32,x64���r���h����
   
   [���C�u����]
   objs\vc2010\Win32\freetype28MT.lib
   objs\vc2010\x64\freetype28MT.lib
   [�w�b�_]
   include\�ȉ��S��


# ---------------------------------------------------------
# libxml2 
#    ftp://xmlsoft.org/libxml2/
# ---------------------------------------------------------
1. libxml2�\�[�X���_�E�����[�h����


# ---------------------------------------------------------
# ANGLE
#    https://github.com/Microsoft/angle
# ---------------------------------------------------------
1. Visual Studio 2015�̕ύX�ŁAWindows 10 SDK(10.0.14393)���C���X�g�[������
   �u�ݒ�v���u�A�v���v���uMicrossft Visual Studio 2015�v���u�ύX�v���u�ύX�v
    ���u���j�o�[�T��Windows�A�v���J���c�[���v�Ƀ`�F�b�N
    ���u���ցv���u�X�V�v

2. ANGLE�\�[�X���`�F�b�N�A�E�g����

3. winrt/10/src/angle.sln���J���ăr���h����




1. �ȉ����|�W�g������`�F�b�N�A�E�g����
   https://github.com/Microsoft/angle
   
   �{�Ƃ�Google�̈ȉ����|�W�g���ƂȂ�
   https://github.com/google/angle
   �������AWindows�Ńr���h����ɂ�Gyp�𗘗p����K�v������
   ���̂��߁AMicrosoft�̃��|�W�g�����痘�p����

2. src/angle.sln���J���ăr���h����
   
   <�r���h�G���[>
   �G���[���e:
     error MSB8036: The Windows SDK version 10.0.14393.0 was not found.
   �Ώ��@:
     �\�����[�V�������E�N���b�N���u�V�������[�V�����̍ă^�[�Q�b�g�v���u8.1�v��I��
   
   �G���[���e:
     include �t�@�C�����J���܂���B'd3d11_3.h':No such file or directory
   �Ώ��@:
     �u�ݒ�v���u�A�v���v���uMicrossft Visual Studio 2015�v���u�ύX�v���u�ύX�v
      �����j�o�[�T��Windows�A�v���J���c�[���́uWindows 10 SDK (10.0.10240)�v�Ƀ`�F�b�N
      ���u���ցv���u�X�V�v


2. winrt/10/src/angle.sln���J���ăr���h����

   VisualStudio���J���ƁA������Ȃ��@�\�̃C���X�g�[����ʂ��o�Ă���̂ŁA
   �u���j�o�[�T��Windows�A�v���P�[�V�����v
   �uWindows 10 SDK 10.0.10240�v
   ���C���X�g�[������iVisualStudio���w�����闬��ɐg��C���āj
   
   Windows10�ȊO�́Asrc/angle.sln���g�p����炵��

2. [Windows10 + VisualStudio2015]
   src/angle.sln���J���ăr���h����

   <�r���h�G���[>
   �G���[���e:
     error MSB8036: The Windows SDK version 10.0.14393.0 was not found.
   �u�ݒ�v���u�A�v���v���uMicrossft Visual Studio 2015�v���u�ύX�v���u�ύX�v
    �����j�o�[�T��Windows�A�v���J���c�[���́uWindows 10 SDK (10.0.10240)�v�Ƀ`�F�b�N
    ���u���ցv

   <�r���h�G���[>
   �G���[���e:
     d3d11_3.h: No Such File or Directory
   �Ώ��@
     �uWindows 10 SDK 10.0.10240�v���C���X�g�[������


