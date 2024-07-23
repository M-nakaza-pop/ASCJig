# github Flow Model

# main------------------mainを残し        -----------mainに戻る
# 　　　branch----------topicで作業-----merge


# branch名をtopicに変える
git branch -m topic  今いるbranchからtopicに変える

git branch -m 旧branch名 新branch名

# [作業]

# 一旦ステージングする
git add r_main.c (file mame)

# commitする
git commit -m "******"

# push
git push r_main.c (file mame)




# originにリポリトジを登録する
git remote add origin https://github.com/M-nakaza-pop/ASCJig.git
# 確認
git remote -v




# Step 1: ローカルブランチ名を変更する
git branch -m master main

# Step 2: リモートリポジトリのmainブランチを認識させる
git remote set-branches origin main

# Step 3: リモートリポジトリのmainを取得し、ローカルブランチmainの上流ブランチに設定する
git fetch --set-upstream origin main

# Step 4: リモートリポジトリのデフォルトブランチをmainに変更する
## リモートリポジトリの情報をもとに自動的に設定する方法（ネットワークアクセス必要）
git remote set-head -a origin
## 明示する方法（ネットワークアクセス不要）
git remote set-head origin main


強制的にpushする
git push -f origin main
