"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[53479],{88722:(e,n,t)=>{t.r(n),t.d(n,{assets:()=>c,contentTitle:()=>a,default:()=>u,frontMatter:()=>o,metadata:()=>r,toc:()=>l});const r=JSON.parse('{"id":"api/Classes/DungeonGeneratorBase/Nodes/GetSeed/GetSeed","title":"GetSeed","description":"Procedural Dungeon \u23f5","source":"@site/docs/api/Classes/DungeonGeneratorBase/Nodes/GetSeed/GetSeed.md","sourceDirName":"api/Classes/DungeonGeneratorBase/Nodes/GetSeed","slug":"/api/DungeonGeneratorBase/GetSeed","permalink":"/ProceduralDungeon/api/DungeonGeneratorBase/GetSeed","draft":false,"unlisted":false,"tags":[],"version":"current","frontMatter":{"slug":"/api/DungeonGeneratorBase/GetSeed","pagination_prev":null,"pagination_next":null},"sidebar":"homeSidebar"}');var s=t(74848),d=t(28453);const o={slug:"/api/DungeonGeneratorBase/GetSeed",pagination_prev:null,pagination_next:null},a="Get Seed",c={},l=[{value:"Inputs",id:"inputs",level:2},{value:"Outputs",id:"outputs",level:2}];function i(e){const n={a:"a",br:"br",code:"code",h1:"h1",h2:"h2",header:"header",img:"img",p:"p",pre:"pre",strong:"strong",table:"table",tbody:"tbody",td:"td",th:"th",thead:"thead",tr:"tr",...(0,d.R)(),...e.components};return(0,s.jsxs)(s.Fragment,{children:[(0,s.jsxs)(n.p,{children:[(0,s.jsx)(n.a,{href:"/ProceduralDungeon/api/",children:"Procedural Dungeon"})," \u23f5\n",(0,s.jsx)(n.a,{href:"/ProceduralDungeon/api/Classes/DungeonGeneratorBase/",children:"Dungeon Generator Base"})," \u23f5\nGet Seed"]}),"\n",(0,s.jsx)(n.header,{children:(0,s.jsx)(n.h1,{id:"get-seed",children:"Get Seed"})}),"\n",(0,s.jsxs)(n.p,{children:[(0,s.jsx)(n.strong,{children:"Class:"})," ",(0,s.jsx)(n.a,{href:"/ProceduralDungeon/api/Classes/DungeonGeneratorBase/",children:"Dungeon\xa0Generator\xa0Base"}),(0,s.jsx)(n.br,{}),"\n",(0,s.jsx)(n.strong,{children:"Category:"})," Dungeon\xa0Generator"]}),"\n",(0,s.jsx)(n.p,{children:"Node"}),"\n",(0,s.jsx)(n.p,{children:(0,s.jsx)(n.img,{src:t(74317).A+"",width:"188",height:"38"})}),"\n",(0,s.jsx)(n.p,{children:"C++"}),"\n",(0,s.jsx)(n.pre,{children:(0,s.jsx)(n.code,{className:"language-cpp",children:"int32 GetSeed() const\n"})}),"\n",(0,s.jsx)(n.h2,{id:"inputs",children:"Inputs"}),"\n",(0,s.jsxs)(n.table,{children:[(0,s.jsx)(n.thead,{children:(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.th,{children:"Name"}),(0,s.jsx)(n.th,{children:"Type"}),(0,s.jsx)(n.th,{children:"Description"})]})}),(0,s.jsx)(n.tbody,{children:(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:"Target"}),(0,s.jsx)(n.td,{children:"Dungeon Generator Base Object Reference"}),(0,s.jsx)(n.td,{})]})})]}),"\n",(0,s.jsx)(n.h2,{id:"outputs",children:"Outputs"}),"\n",(0,s.jsxs)(n.table,{children:[(0,s.jsx)(n.thead,{children:(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.th,{children:"Name"}),(0,s.jsx)(n.th,{children:"Type"}),(0,s.jsx)(n.th,{children:"Description"})]})}),(0,s.jsx)(n.tbody,{children:(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:"Return Value"}),(0,s.jsx)(n.td,{children:"Integer"}),(0,s.jsx)(n.td,{children:"Get Seed"})]})})]})]})}function u(e={}){const{wrapper:n}={...(0,d.R)(),...e.components};return n?(0,s.jsx)(n,{...e,children:(0,s.jsx)(i,{...e})}):i(e)}},74317:(e,n,t)=>{t.d(n,{A:()=>r});const r="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAALwAAAAmCAYAAAB+tPGOAAALK0lEQVR4Xu1cW08VaRZdhVeQmwKiKKLgtRWZVrz02Npq4hjjLU5aHW19MBljnPHBXzDxoR3nyYeZiZNOvKRb2wztRI2Oitqtjpe2sVUYUFsJIIqCoMilEZBbzV7foco6B2g4B8ZTB2onFeDUrlNf7VrfrrXXtwsNXbNocVsi29GuuTteTgTeawQ2ytm+k62ss7NqnTh8JPu/j4mJQWpqKiZMmIDw8HAEBwdjyJAhaG5u7uz7nf1OBHo8AvX19Xj9+jXq6upQUlKCR48e4enTp6ipqeG5PpbtRkcn7Qjww+WA0qSkJKxYsQLr1q3D8OHDFcBzcnJw//595OXl4dmzZz1+Mc4XOhHoLAJMuqNGjcLUqVMRGxuLhoYGZGZmIj09HQUFBaitreVXxLaX8dsD/GxxzFizZg127tyJYcOGIS0tDefPn+9sHM5+JwJ+i0B8fLwC/93Muyh6WoTi4mKOZY5st6yD8gT8vIEDB15fvXo1tm3bhgcPHmD//v1obnGoi9/upHPiLkdA0zRMnDgRDx8+RHVVNZ4/fw5d190ojhXwLExfpqSkYPfu3SqjnzlzpssncxydCNglAgkJCSgsLFSc/uXLlxxWjGyv+IsV8HpYWBj27NmDO3fuOBTGLnfPGYdPESDFKSoqQlVVFd68eWNivX/rty3s378/ODM4I06fPu3TSZyD/BsB1l1U0gzbu3cvmpqa/DsoP509Pz8fiYmJoKLDTQSXhTKUKwbgLwt3x9atW7Fr1y60tLT4aZj2Om2/fv1AijdlyhSlUg0YMEBJYSyIqApQEbCTUUWzSsWNjY19FvC8L7m5ueTwILblvl0moyHgx5DscyNvb2hssNM99NtYSO/Wr1+vgG614JBgJI1PUlt2draqcxhUOxgTVYv+Llk1NTeBW1+2EbEjUFxSrPAt92kMAf+EmWzkyJG4fJmTwLGgoCCsW7sOMdEx0Fs6BnPytGSlBly5csUWQSPgreNtaZYJIFtfNj6NmZB4T+Xp90RRGqJ/xowZzkJSKzImT57sltnLyspw8uRJVFZWKl64cuVKDBo0SHnPnTsXGRkZxmKHrbDFG22Xp4+/A9MKeAQZgOcSrWOuCIwdO9YtFOfOnQNBzxU9arzXr1839/PpOG7cOCd0No4ABRkmdZqZ4dkq4JgrAoMHD3YLxatXSsI1jXKX1bjU7WlcoZ4/f76aPCEhIUoao3Jw7do1VFdXd9ufvU2zZs1CREQEysvLcfHiRef2dSEChL1uoL8L/m1c9MSVUvZKI2XMh0B4AlD9RJavMoGnF6EVBKa8uWzZMsyezQ4Ll7G1gpndMD4eIyMjzb/Zu0HpyzBm/I0bN4KZxdPoe+jQIVgnkbf+S5cuVVTKalRnOJGGDh1qfswFxECQJbVF44GPxkL7QNpf4iKA4iroD0qBm4XQL/dcIia96yfR2eUr4PXp24HJnwFNcrMLZVU2629A7QsBvjzi4xcDA8Ohld72ZR759RgGZvr06eYYxo8fj4qKCmPVTvFiypPGZgUVs/mWLVtMju95IZQ22fh09+5dtctbfx67atWqNvHhJGQXq9X4NLG7xKz97kNoK6dCa2iG/p886EfuyJpoDbTRkdDmSAINlVrpnmCqh8xnwKvMTrAT0D/+GSj5HqiR7smXWfK7dGeGxQvoFwEN1dCipgH15UCjWvGyvRHccXFxiIqKUmNlpmZnHrtHCXK2pnZUDC5YsED5GUb+f/z4cfUdo0ePVh+TAlHDZ0b21n/JkiWqScqwCxcuqCcQx8w+EmvysjvgmdkJdl0ArX8h+MkSRaX0Z+BhGfRMwdIISZhzxgBvGqBNkM6XyjqgrrFb+FFFq09GGlMjHWnM6nXSr2Dov/xZJ5w366+u/fQLGgD9N18Kcty5sU/nfU8HHTt2zMzCxim5XE1tnl2k5NDtPRk5MQxjoXvr1i1Fd27ccG/RNgpdb/2tK6lUjW7evAkuMNlxIazTWyU0BmU10L+WrF5RZ0qqSlrl30ckmcp+0h3074egvywHBrWliZ2ex+LgO+DJ2V9kuDJ3e1b/2rWffm8r1U998ReigfKhYn8jTWGLxcGDB1WxaTUuSi1fvhybN292K3BJKawc2vq+ABuZrKugLGq99eeTwVpQs0HKaoHA163jJWfXsyUpMnN7GFc/9Kp6tV9x+5+FNk+JRdCupZJAO3tvqWN8+T5dWKC+ynmX2duMWDL9q2xXIUvA0xJksBM+lTXfNPsjvnWEVGSOHDmi6A1VkZkzZ5rFKLP02rVrcfjwYeXtqdawDmBbgmGUMA2jju+tPyea1dgYFdAmBaqeK+ygdXFPKSiWC9Lkc+7XWMhWt4oCH4+DtnQy9HM/+XTpvmd4qjExKZKxO/gKfh4thR/9BoS4BvdEXiLJO+7TQP19EKU/vlGzb98+JQMaxoUog56wZ8Nq5O3M4sZm3Uc65K0/C16rvX371t9h6d75RY3RJknrhiVjE/TGxs+1idLZK34Ibo3t9cfQv831+by+Z3hKj7GzgPyTosyIhORpg6XIGCEvnNCPvwvYtW9/L7O5e0WHz1fqxYHJyckmPydP5vuShrE4ZCG6adMm8zMWqY8fP24jAfLp4Ek7jIPYlepJQTrz91Rc2pM9vbhMv7tSetSSR0L/TgBcrl7Lc7ehwdCmx7kkykip/wTsLX9KF/HD9xeSzIUnr5egRWcHZcmUHcB//+4qVHUZCDl6sACcn4fGAdn/EI6WB40TIwDAzoizdcDIpuTvpDRWKy11n+ChoaFqd2vftdvkuHTpUofA8qQ0nEy/5B8dzXd03hlfpA9ou1kIUJb8bKarcCWXbxZSw4xPsMvnGC6x/Wcm9CcV0C+JJt8NsDNWCvDssJOHh1ex46KSHirSY+IK4Ne7RZ780cXpo5NdmZ9gL/h3QC4+MavzPzXQqHsT/FRCDDPkSuNvthzQKDOyODUmAPV7ZuGOiklv/SmHsvA1aoExY0SyC2BTi0qUHhcmQdv5CfScEhdnFxrDzE+w61fye2TxSaoBFSmlw3uJ9XchJsgbRDel5s6CdJpQljApUmuEAuR+I3qqe2YMlHsTER4BA0wEbNyoODCrkzOzXZhPAAPUvKYfMn5AaZkr64eFhyF+tMRDjBMlKjoKz4ufqwkTOTQSqTNTsWjxImRlyXqFl/58CnNcVHjUsVLEVlRWqFVbypXz5s0zJwP3X7121fYLT/o96eESnV0BnwXppylSpIZDL6mGnv4T9NP3exQ27wpj7xJ8jw7Cbl8WEhyC7X/YjrBQd1WkvXGWvy5XhawhOXIi7Pjjjjb9OJ7HHjhwAEXPitTE8cZ/0sRJ2LBhQ5dC9vnuzwOitaBLF9Ndp1b5x5RYfG0v6O447Hh8bV0tvvryK7d+l/bGSc599Oujbvo6KU3aN2luFMjz2Pq39aa06a3/o9xHyM4Rubcd4z8kcuyXI+CiNGKKw9vjxR1b3DM2ebHfhVo3qQl1c3Jn0hq2UrMH/tSpU8Z/u3Ibc2VFJe7du6eOY68Mf5Lnv3jxArdv38aJEyfMvhwe6K0/ga0okjSw8bspk549e1a90sb3Ggy7etX+lOZ932wSGRHT4SKUjjkR6N0R+JXB3J3c3rtvtHN1rgiYy6TrnYg4EejlEVAYd/tHTL38gp3L69sRUFi3NsLIapFjTgR6ZQRMbFsBz1eT1vbKy3Uuqi9HgJg2X7trb7lJ+ndxrC9HyLn2XhMBgv1f1qvpaH01VZykb8AxJwIBGwHSmDYvVHfUD09HTobfBuzlOgPvqxEgZonddv97QFc7aD6QL+jZLp6+ejuc6/5/RYAvEz/o7Mv/B7RRwErcUp4sAAAAAElFTkSuQmCC"},28453:(e,n,t)=>{t.d(n,{R:()=>o,x:()=>a});var r=t(96540);const s={},d=r.createContext(s);function o(e){const n=r.useContext(d);return r.useMemo((function(){return"function"==typeof e?e(n):{...n,...e}}),[n,e])}function a(e){let n;return n=e.disableParentContext?"function"==typeof e.components?e.components(s):e.components||s:o(e.components),r.createElement(d.Provider,{value:n},e.children)}}}]);