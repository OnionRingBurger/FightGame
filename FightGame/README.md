# FightGame

�u�ԍ����v���R���Z�v�g�ɂ��� 3D �A�N�V�����o�g���Q�[���ł��B
DirectX 11 �Ǝ��� ECS�iEntity Component System�j��Ղ̏�ɁA��`�̍U������E���[�e�B���e�B AI�E
�V�[���iWorld�j���Ƃ� System ���s���Ƃ����\���ō���Ă��܂��B

- ���� / ��: C++17 / Visual Studio / Windows x64
- �`��: DirectX 11�i����`���� + ���� HLSL �V�F�[�_�j
- ����R�[�h�K��: �� 214 �t�@�C�� / �� 26,600 �s�iimGui�Eassimp�EDirectXTex ���̊O�����C�u�����������j

---

## 1. ���̃Q�[���ł���Ă��邱��

### �R���Z�v�g: �L���Ȋԍ�����T���āA�ێ�����

���荇���̏�肳�ł͂Ȃ��A**�ǂ̋����ɗ���**�ŏ��s�����܂����_���Ă��܂��B

- �v���C���[�̍U���́u�ߋ����i�˒� 1.7 / �� 160���j�v�Ɓu�������i�˒� 6.2 / �� 60���j�v�� 2 ���
- �G�̍U���͂��ꂼ��L���ȋ����т��Ⴄ�i��: �˒� 2.5 �̑S���U���A1.3?3.7 �̒������A3.5?7.5 �̑_���j
- �܂�u�G�̍U�����͂��Ȃ����A�����̍U���͓͂��сv�����݂���B�����ɗ��Ă�Έ���I�ɉ����
- �t�ɓG�̓��Ӌ����ɓ���ƁA������͑I�����������ĉ����؂���

�U���f�[�^�� `Assets/Data/AttackData.json` �ɊO�o������Ă���A�˒��E��̊p�x�E�_���[�W�E
�O���iStartupTime�j�E�㌄�iWaitTime�j�E�����iLifeTime�j�� JSON �Œ����ł��܂��B

### �΍R�A�N�V����

�G�̍U���ɑ΂��āA�ς��邩�����邩��I�т܂��B

- **�K�[�h**: �����Ă���ԃ_���[�W���y������B�h������Ȃ��̂őϋv�ɂ͌��E������
- **�W�����v**: ������ɓ����Ĕ�����O���B��������΃m�[�_���[�W�����A�^�C�~���O�̓V�r�A

�ǂ�����u�U����H�炤�ʒu�v�����łȂ��u�M���M��������Ȃ��ʒu�v�ł��������邽�߁A
�G�̍U���͈͂ƃ^�C�~���O�����؂�΁A���X�N�𕉂킸�ɗ������܂��B

### �G

�G�͖��t���[���ꓖ����ɓ����̂ł͂Ȃ��A**�ʒu�֌W�̖ڕW�iStance�j��������**�s�����܂��B

- �s���E�o / �L���ێ� / �j���[�g�����l�q�� �� 3 �̖ڕW���A�����E��e���E�p�����Ԃ���X�R�A�����O���đI��
- �I�񂾖ڕW�̉��ŁA�ڋ߁E���ԁE�Z 0�E�Z 1�E�ҋ@ �Ȃǂ̎�i�� Behavior Tree �Ŏ��s����
- �����Z��A�ł��Ă��Ȃ����i`sameAttackStreak`�j�Ȃǂ́u�؋��v�𗭂߁A臒l�𒴂�����ڕW���ĕ]������

### ���b�N�I��

��苗�����̓G�������ŕߑ����A�������a�̊O�ɏo��܂œG�̕����iyaw �̂݁j�����������܂��B
���������킹�鑀��R�X�g���O�����ƂŁA�v���C���[�̒��ӂ������̑I���ɏW�߂�̂��ړI�ł��B
�ߑ� 7.0 / ���� 7.8 �̃q�X�e���V�X���������A�������U���̎˒����Ń��b�N���؂�Ȃ��悤�ɂ��Ă��܂��B

### ���^�V�[��

Startup / Load / Title / Tutorial / Proto / Result / Clear �̊e World ���q�����Ă���A
�ԍ����i���̖{���� **Proto** �ł��B�ق��ɊJ���p�� Test �ƁA�X�e�[�W�ҏW�p�� CreateGame ������܂��B

---

## 2. ������@

### ��{����

| ���� | �L�[�{�[�h / �}�E�X | �Q�[���p�b�h�iXInput�j |
| --- | --- | --- |
| �ړ� | `W` `A` `S` `D` | ���X�e�B�b�N |
| �J������] | �}�E�X�ړ� | �E�X�e�B�b�N |
| �ߋ����U�� | �E�N���b�N | `B` |
| �������U�� | ���N���b�N | `A` |
| �W�����v | `E` | `RB` |
| �K�[�h | `Q`�i�����Ă���ԁj | `LB`�i�����Ă���ԁj |
| ���b�N�I���Ώۂ̐ؑ� | �}�E�X��傫�������� | �E�X�e�B�b�N |

- �ړ����͂̓J������ yaw �Ń��[���h�����ɕϊ�����邽�߁A�u������v�̓J������̌�ނɂȂ�܂�
- ���X�e�B�b�N�ƃL�[�{�[�h�͓��͗ʂ̑傫�����������̗p����܂��i�����ڑ��ł��������܂���j
- �ߋ����U���Ɖ������U���𓯎��ɉ������ꍇ�͋ߋ����i�E�N���b�N / `B`�j���D�悳��܂�
- ���b�N�I���ɐ�p�{�^���͂���܂���B�ߑ��Ɖ����͋����Ŏ����A�Ώۂ̐ؑւ������蓮�ł�

### �V�[������E�f�o�b�O

| ���� | ���蓖�� | ���e |
| --- | --- | --- |
| ����i�^�C�g���j | `Space` / `A` | �V�[���J�n |
| ���g���C | ���N���b�N / `A` | �Q�[���I�[�o�[��̍ĊJ |
| �`���[�g���A���X�L�b�v | `Enter` | �`���[�g���A����΂� |
| ImGui �\���ؑ� | `F2` | �f�o�b�O UI �̊J�� |
| �f�o�b�O�V�[���� | `F3` | �f�o�b�O�p�V�[���J�� |
| �f�o�b�O�R���\�[�� | `F1` | �R���\�[���\�� |
| �I�� | `Esc` / `START` | �Q�[���I�� |

`F1` / `F2` / `F3` �� Debug �r���h�ł̂ݗL���ł��B
�f�o�b�O�V�[���ł̓J������ `WASD` �{ �}�E�X�Ŏ��R�ɓ������܂��B

---

## 3. �Z�p�I�A�s�[��

### 3-1. ���� ECS ��Ձi`include/ECS/Core`�j

�O���� ECS ���C�u�������g�킸�AEntity / Component / System / Chunk / World �����삵�Ă��܂��B

- **�^���Ƃ̖��ȃX�g���[�W**: `ComponentStorage<T>` �� `unordered_map<Entity, index>` +
  `vector<ComponentPack>` �������A�폜�� swap-and-pop �� O(1)�BComponent ���̂͘A���z�u����܂�
- **�^���X�g�ɂ��N�G��**: `chunk.GetView<ComponentTypes<Position, Velocity>>()` �̂悤��
  �K�v�� Component ���e���v���[�g�����œn���ƁA���̑S�Ă����� Entity ������񋓂��� View �������܂��B
  �� 2 �����ɏ��O�p�̌^���X�g��n���΁uA ������ B �������Ȃ��v�������܂�
- **�ǂݏ������^�ŋ�ʂ��� `ComponentHandle`**: `handle->member`�i�������݁j��
  `handle.Look().member`�i�ǂݎ���p�j�𕪂��A�������ݑ��������ύX�t���O�𗧂Ă�݌v�ɂ��Ă��܂�
- **�^���������X�g���[�W�Ǘ�**: `StorageManager` �� `TypeID �� IComponentStorage` �ŕێ����A
  ���߂Ă��̌^�� `AddComponent` �������_�ŃX�g���[�W�𓮓I�ɐ������܂�
- **Entity �� id + generation**: �폜���� generation ��i�߂邱�ƂŁA
  ID ���ė��p���Ă��Â��Q�Ƃ��L�������Ȃ��悤�ɂ��Ă��܂�
- **System �͏�Ԃ������Ȃ����R�֐�**: `void FooSystem(Chunk&, const SystemContext&)` �̌`�ɓ���B
  ���s���̐Ӗ��� System ���Ɏ��������AWorld �̌Ăяo�����������B��̐^���ɂȂ�܂�
- **World �����s���𖾎���**: ���񂾂��K�v�ȏ����iPose �m��Ȃǁj�� `InitChunk` �ɁA
  ���t���[���̏����� `UpdateChunk` �ɕ����A�V�[�����ƂɃp�C�v���C����g�ݑւ����܂�
- ���� 147 �� Component ��`�A95 �� System �֐��A24 �� System �w�b�_�ō\������Ă��܂�

### 3-2. ���W�X�V���u���ʃR���|�[�l���g�v�ɏW�񂵂��ړ��p�C�v���C��

�����̈ړ������������t���[���� `Position` �𒼐ڏ���������ƁA�㏑�������ł��������N���܂��B
�����Ŋe�ړ� System �͍��W�𒼐ڐG�炸�A**���ʂ��������ނ���**�ɂ��܂����B

| �R���|�[�l���g | ���� | �������ޑ� |
| --- | --- | --- |
| `FixedResult` | ��΍��W�E��Ίp�x�̌��� | �Ǐ]�A�����A���[���A���� |
| `MotionResult` | ���̃t���[���̈ړ��E��]�̍����i���[�v�w����j | ���͈ړ��A�O�i�A���� |
| `EphemeralResult` | 1 �t���[������̃I�t�Z�b�g�i�h��Ȃǁj | ��ʗh��A�U�� |
| `MotionTransform` | `MotionResult` �̍������t���[�����ׂ��ŗݐ� | Pose �n System |

������ `PoseSystem` �� 1 �����ō������A`TransformSystem` �� `Position` / `Rotation` �֔��f�A
`ResetSystem` �����ʂ�|�����܂��B�J������Ǐ]�̂悤�Ɂu��Ŋm�肳�������v�Ώۂ�
`LatePoseSystem` �ɉ񂷂��ƂŁA����t���[�����̈ˑ������\���ł��܂��B

### 3-3. Component �� JSON �V���A���C�Y�i�^���̎菑�����t���N�V�����j

`ComponentField.h` �� Component ���Ƃ� `ApplyToFields(component, valueFunc, entityFunc)` ��p�ӂ��A
�t�B�[���h���ƒl��K��ł���悤�ɂ��Ă��܂��B���̖K��֐��ЂƂŁA

- `nlohmann::json` �ւ̕ۑ��i`TypeId` / `Version` / `TypeName` + `data` �K�w�j
- JSON ����̕����i`TypeID` ���L�[�ɂ��������֐����W�X�g���o�R�� `std::any` �Ƃ��Ď󂯎��j

�̑o�������������Ă��܂��B**Entity �Q�Ƃ͂��̂܂ܕۑ�����Ɖ���**���߁A
�ۑ����� Entity �z����̃C���f�b�N�X�֕ϊ����A�ǂݍ��ݎ��ɐV���� Entity �֒��蒼���Ă��܂��B
Chunk �P�ʂ̕ۑ��E�ǂݍ��݁i`OutJsonComponents` / `LoadJsonComponent`�j�܂Œʂ��Ă��܂��B

### 3-4. �O�w�\���̓G AI�iUtility + Behavior Tree�j

`include/AI` �� 23 �t�@�C���Ŏ���� Behavior Tree �� Utility �]�����������Ă��܂��B

- **�w�̕���**: Role�i�������J�[�u�W���j�� Stance�i�ڕW�j�� Action�i��i�j�� 3 �i�ɕ����A
  �ω����鑬���𑵂��Ȃ��悤�ɂ��Ă��܂��i�����͂������A�ڕW�͂Ƃ��ǂ��A��i�͖� Tick�j
- **Utility �ɂ��ڕW�I��**: `StanceNode` �������E��e���E�p�����Ԃ���X�R�A���o���� argmax �����܂��B
  �p���{�[�i�X�́u���݂� Stance �̃X�R�A�ɂ����v�|����i�S���Ɋ|����� argmax �őł�������邽�߁j
- **���i�� 0?1 �̃m�u�ŕ\��**: `RoleCurveKnobs`�i�����̂������A�����̍D�݁A��e���x�A�ڕW�̔S��j��
  �����̌W���փ}�b�s���O����w�����݁A�G�̐��i�����W�����������ō���悤�ɂ��Ă��܂�
- **�f�[�^�̒u�����Ӗ��ŕ���**: `AIBlackboard`�i���́j/ `AIMind`�i�v�l�̏�ԂƋL���j/
  `AIContext`�i�؂̐i�s�x�j/ `AIResult`�i�� Tick �̏o�́j�BSystem ���� `AIResult` ���������܂�
- �m�[�h�Q�� `Selector` / `Sequencer` / `ReactiveSelector` / `Decorator` / `ActionNode` �Ȃǂ�����
- **AI �̎v�l�� 3 �t���[���� 1 ��**�ɊԈ����A�ړ��ⓖ���蔻��̓t���[�����ɉ񂵂Ă��܂��B
  �v���C���[���͂��G AI ������ `MoveInputResult` �ɏW�񂳂�邽�߁A
  ���͌��i�f�o�C�X / AI�j�������ւ��邾���œ����ړ��E�U���o�H�����L�ł��܂�

### 3-5. �`��

- **��`���b�V���̓��I����**: �U���͈͂̐���p�V�F�[�_�ŕ`�悵�܂��i`Geometory::DrawSector`�j�B
  �����a�E�O���a�E�p�x�E�������璸�_�𐶐����A�i�s�x�ɉ����ĐF��ς����邽�߁A
  �u�ŏ��͔����A�U�����߂Â��ƐF���t���v�\���\���Ɏg���܂�
- **�U���f�[�^����̎����o�^**: `AttackData.json` �̃L�[�𑖍����Đ�b�V�����ꊇ�o�^���邽�߁A
  �U���� JSON �ɑ����Ε`�摤�̒ǉ���Ƃ͕s�v�ł�
- **�V�F�[�_�͎��s���R���p�C��**: HLSL ���\�[�X���Ɏ��� `D3DCompile` �ŋN�����ɐ������܂��B
  Lambert / Specular / Toon / Fog / Ghost / �A���t�@�u�����h�A�{�[���Ή��̒��_�V�F�[�_��p��
- **�����蔻��**: OBB �̐����E����E�����߂��ɉ����A���C�Ɛ�`�̔��������
- **�A�j���[�V����**: Assimp �̃X�P���^���A�j���i�ő� 200 �{�[���j�ƃ��[�t�^�[�Q�b�g�ɑΉ�
- **�񓯊����[�h**: ���f���E�e�N�X�`���E�G�t�F�N�g�E�T�E���h�����[�J�[�X���b�h��
  �X���b�h�Z�[�t�L���[�œǂݍ��݁A���[�h��ʂƕ��s�����Ă��܂�
- ���f���ǂݍ��݂� Assimp�A�e�N�X�`���� DirectXTex�A�G�t�F�N�g�� Effekseer�A
  �����`��� DirectWrite�A������ XAudio2 ���g�p

### 3-6. �f�o�b�O�� / �ȈՃX�e�[�W�G�f�B�^

ImGui ��� ECS �̒��g�����̂܂ܕҏW�ł���悤�ɂ��Ă��܂��i`ImGuiSystem` / `Chunk::ImGuiInPut`�j�B

- Entity �̐V�K�쐬�E�폜�AComponent �̒ǉ��E�폜�A���O���ʂł̌���
- �I������ Entity �̑S Component ���A3-3 �� `ApplyToFields` ���ė��p���ăt�B�[���h�P�ʂŕҏW
- Chunk �S�̂� JSON �ɕۑ� / �C�ӂ̃L�[����ǂݍ��݁i�f�o�b�O��p Entity �͕ۑ��ΏۊO�j
- �f�o�b�O�J�����A�f�o�b�O�p�V�[���A�R���\�[���o�͂̐؂�ւ�
- Component ��`�� `kTypeId` / `kTypeName` / `kVersion` ���������AImGui �\���ƃV���A���C�Y�ŋ��p�B
  TypeID / TypeName �̏d���� ImGui ��Ō��o�ł��܂�

�܂�**�ҏW UI �ƃZ�[�u�f�[�^�������K��֐� 1 �{���琶������Ă���**�̂��A���̊�Ղ̗v�_�ł��B
Component �� 1 �����΁A�ۑ��E�����EImGui �ҏW�������ɕt���Ă��܂��B

### 3-7. �g�p���C�u����

Assimp�i���f���j�ADirectXTex�i�e�N�X�`���j�AEffekseer�i�G�t�F�N�g�j�ADear ImGui�i�f�o�b�O UI�j�A
nlohmann/json�i�V���A���C�Y�j�ADirect2D / DirectWrite�i�����j�AXAudio2�i�����j�AXInput�i�p�b�h�j�Azlib�B
�`��E���́EECS�EAI�E�V���A���C�Y�̑w�͎���ł��B

---

## 4. �r���h

1. `FightGame.sln` �� Visual Studio �ŊJ��
2. �\���� `Debug` �܂��� `Release`�A�v���b�g�t�H�[���� `x64` �ɂ���
3. �r���h���Ď��s�iC++17 / Windows 10 SDK�j

�O�����C�u������ `Lib/` �z���̐ÓI���C�u�����������N���Ă��܂��B

---

## 5. �f�B���N�g���\��

| �p�X | ���e |
| --- | --- |
| `include/ECS/Core` | ECS ��ՁiChunk�A�X�g���[�W�AView�A�V���A���C�Y�j |
| `include/ECS/Component` | Component ��`�i`Components.h`�j |
| `include/ECS/System` | System �錾 |
| `include/ECS/World` | �V�[���iWorld�j��`�� System ���s�� |
| `include/AI` | Behavior Tree / Utility �]�� / �e�m�[�h |
| `Source/Drawer` | �`���Ձi`Geometory` �Ȃǁj |
| `Assets/Data` | �U���f�[�^�Ȃǂ� JSON |
| `docs/` | �d�l�E�݌v�����i�Ӑ}�̃h�L�������g�j |

�݌v�Ӑ}�̏ڍׂ� [`docs/README.md`](docs/README.md) ��ڎ��Ƃ��āA
[`docs/game-overview.md`](docs/game-overview.md)�A[`docs/ai-behavior.md`](docs/ai-behavior.md)�A
[`docs/lock-on.md`](docs/lock-on.md) �ɂ܂Ƃ߂Ă��܂��B

---

## 6. ����Ɩ�����

����r���̂��߁A�ȉ��͖������ł��i�ڍׂ� [`docs/implementation-tasks.md`](docs/implementation-tasks.md)�j�B

- �G�U���̗\���\���i���� �� �U���J�n�ŐF���t���j�͕`�摤�̎d�g�݂̂ݗp�Ӎς�
- �΍R�A�N�V�����������̋����i��������t����i�K�j�͖�����
- ���������E�X�e�[�W�J�ځE���U���g�܂ł̃Q�[�����[�v�ڑ�
- �q�b�g�G�t�F�N�g�A�A�j���[�V������ Model ����̕���
- `ComponentStorage` �̕ύX���m�iversion / changedEntities�j�͘g�݂̂ŁA���݂͖��g�p
- ���[���V���[�^�[�n�� `GameWorld` �͍X�V��������̃X�^�u�i�{���� Proto ���j

---

## 7. �N���W�b�g

�g�p�������f���E�����̃N���W�b�g�� [`Credit.txt`](Credit.txt) ���Q�Ƃ��Ă��������B
