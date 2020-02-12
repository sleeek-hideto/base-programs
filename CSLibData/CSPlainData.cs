using System;
using System.Collections.Generic;
using System.Text;

using System.IO;
using System.Text.RegularExpressions;

/* @brief How to Use
	string tmp;
	CSPlainData3 data = new CSPlainData3();

	data.SetNameSpace("P");
	data.Set(0, "hoge", "Hello World!");
	data.Set(1, "paka", new int[3] { 1, 2, 3 });
	data.Set(0, "paka", new float[2] { 1.0f, 2.0f });
	data.Set(-1, "aaa", "asida");
	data.Set(-1, "bbb", new bool[2] { true, false });
	data.Save("../../test.dat");

	data.Load("../../test.dat");
	data.Get(0, "hoge", out tmp);
	Console.WriteLine(tmp);
	data.Get(3, "poka", out tmp);
	Console.WriteLine(tmp);
*/

namespace CSLibData
{
    public class CSPlainData_Node
    {
        public String m_IndexName;
        public int m_IndexNumber;

        public CSPlainData_Node m_NodeChildTop, m_NodeChildBottom;
        public CSPlainData_Node m_NodePrevious, m_NodeNext;

		public CSPlainData_Node[] m_CacheIndexNumber;

		private void AddChild(CSPlainData_Node node)
		{
			if (m_NodeChildTop == null)
			{///// 空
				m_NodeChildTop = node;
				m_NodeChildBottom = node;
				node.m_NodePrevious = null;
				node.m_NodeNext = null;
				return;
			}

			if (node.m_IndexNumber > m_NodeChildBottom.m_IndexNumber
				|| String.Compare(node.m_IndexName, m_NodeChildBottom.m_IndexName) > 0)
			{///// 最後
				m_NodeChildBottom.m_NodeNext = node;
				node.m_NodePrevious = m_NodeChildBottom;
				node.m_NodeNext = null;
				m_NodeChildBottom = node;
				return;
			}

			if (node.m_IndexNumber < m_NodeChildTop.m_IndexNumber
				|| String.Compare(node.m_IndexName, m_NodeChildTop.m_IndexName) < 0)
			{///// 最初
				m_NodeChildTop.m_NodePrevious = node;
				node.m_NodePrevious = null;
				node.m_NodeNext = m_NodeChildTop;
				m_NodeChildTop = node;
				return;
			}

			CSPlainData_Node pBack = m_NodeChildTop;
			while (pBack != null)
			{///// その他
				if( (node.m_IndexName == null && node.m_IndexNumber < pBack.m_IndexNumber)
					|| (node.m_IndexName != null && String.Compare(node.m_IndexName, pBack.m_IndexName) < 0 ) )
				{
					CSPlainData_Node pPre = pBack.m_NodePrevious;
					pPre.m_NodeNext = node;
					node.m_NodePrevious = pPre;
					node.m_NodeNext = pBack;
					pBack.m_NodePrevious = node;
					return;
				}
				pBack = pBack.m_NodeNext;
			}
		}

		public CSPlainData_Node SearchNode(int number)
		{
			if (m_NodeChildTop == null || m_NodeChildTop.m_IndexName != null) return null;

			if (m_NodeChildTop.m_IndexNumber > number) return null;
			if (m_NodeChildTop.m_IndexNumber == number) return m_NodeChildTop;
			if (m_NodeChildBottom.m_IndexNumber == number) return m_NodeChildBottom;
			if (m_NodeChildBottom.m_IndexNumber < number) return null;

			if (m_CacheIndexNumber != null)///// キャッシュを使う
			{
				int bias = number - m_NodeChildTop.m_IndexNumber;
				return m_CacheIndexNumber[bias];
			}

			CSPlainData_Node pIte = m_NodeChildTop;
			while (pIte != null)
			{
				if (pIte.m_IndexNumber == number) return pIte;
				pIte = pIte.m_NodeNext;
			}
			return null;
		}

		public CSPlainData_Node SearchNode(String name)
		{
			if (m_NodeChildTop == null || m_NodeChildTop.m_IndexName == null) return null;

			if (String.Compare(name, m_NodeChildTop.m_IndexName) < 0) return null;
			if (m_NodeChildTop.m_IndexName == name) return m_NodeChildTop;
			if (m_NodeChildBottom.m_IndexName == name) return m_NodeChildBottom;
			if (String.Compare(name, m_NodeChildBottom.m_IndexName) > 0) return null;

			CSPlainData_Node pIte = m_NodeChildTop;
			while (pIte != null)
			{
				if (pIte.m_IndexName == name) return pIte;
				pIte = pIte.m_NodeNext;
			}
			return null;
		}

		public CSPlainData_Node BearNode(int number)
		{
			CSPlainData_Node pNode = SearchNode(number);
			if (pNode != null) return pNode;

			pNode = new CSPlainData_Node();
			pNode.m_IndexName = null;
			pNode.m_IndexNumber = number;
			AddChild(pNode);
			return pNode;
		}
		public CSPlainData_Node BearNode(string name)
		{
			CSPlainData_Node pNode = SearchNode(name);
			if (pNode != null) return pNode;

			pNode = new CSPlainData_Node();
			pNode.m_IndexName = name;
			pNode.m_IndexNumber = 0;
			AddChild(pNode);
			return pNode;
		}
		public CSPlainData_Leaf BearLeaf(int number)
		{
			CSPlainData_Leaf pNode = (CSPlainData_Leaf)SearchNode(number);
			if (pNode != null) return pNode;

			pNode = new CSPlainData_Leaf();
			pNode.m_IndexName = null;
			pNode.m_IndexNumber = number;
			AddChild(pNode);
			return pNode;
		}
		public CSPlainData_Leaf BearLeaf(string name)
		{
			CSPlainData_Leaf pNode = (CSPlainData_Leaf)SearchNode(name);
			if (pNode != null) return pNode;

			pNode = new CSPlainData_Leaf();
			pNode.m_IndexName = name;
			pNode.m_IndexNumber = 0;
			AddChild(pNode);
			return pNode;
		}

		public void MakeCacheNumber()
		{
			if (m_NodeChildTop == null) return;

			if (m_NodeChildTop.m_IndexName != null)
			{
				m_CacheIndexNumber = null;
				return;
			}

			int lenCache = m_NodeChildBottom.m_IndexNumber - m_NodeChildTop.m_IndexNumber +1;
			if (lenCache < 1) return;
			m_CacheIndexNumber = new CSPlainData_Node[lenCache];

			CSPlainData_Node node = m_NodeChildTop;
			while (node != null)
			{
				int bias = node.m_IndexNumber - m_NodeChildTop.m_IndexNumber;
				m_CacheIndexNumber[bias] = node;
				node = node.m_NodeNext;
			}
		}

		//public CSPlainData_Node SearchNodeByCache(int number)
		//{
		//    if (m_CacheIndexNumber == null) return null;
		//    int bias = number - m_NodeChildTop.m_IndexNumber;
		//    return m_CacheIndexNumber[bias];
		//}
	}

	public class CSPlainData_Leaf : CSPlainData_Node
    {
        public enum _TypeData { STRING, INT, FLOAT, BOOL };
		public _TypeData m_TypeData;
        public String m_DataString;
		public int[] m_DataInt;
		public float[] m_DataFloat;
		public bool[] m_DataBool;

		public void SetData(String data)
		{
			m_TypeData = _TypeData.STRING;
			m_DataString = data;
		}
		public void SetData(int[] data)
		{
			m_TypeData = _TypeData.INT;
			//m_DataInt = new int[data.Length];
			m_DataInt = (int[])data.Clone();
		}
		public void SetData(float[] data)
		{
			m_TypeData = _TypeData.FLOAT;
			m_DataFloat = (float[])data.Clone();
		}
		public void SetData(bool[] data)
		{
			m_TypeData = _TypeData.BOOL;
			m_DataBool = (bool[])data.Clone();
		}

		public int GetData(out String data)
		{
			data = null;
			if (m_TypeData != _TypeData.STRING || m_DataString == null) return 0;
			data = m_DataString;
			return m_DataString.Length;
		}
		public int GetData(out int[] data)
		{
			data = null;
			if (m_TypeData != _TypeData.INT || m_DataInt == null) return 0;
			data = (int[])m_DataInt.Clone();
			return m_DataInt.Length;
		}
		public int GetData(out float[] data)
		{
			data = null;
			if (m_TypeData != _TypeData.FLOAT || m_DataFloat == null) return 0;
			data = (float[])m_DataFloat.Clone();
			return m_DataFloat.Length;
		}
		public int GetData(out bool[] data)
		{
			data = null;
			if( m_TypeData != _TypeData.BOOL || m_DataBool == null ) return 0;
			data = (bool[])m_DataBool.Clone();
			return m_DataBool.Length;
		}

		public String PrintLine()
		{
			string str;
			StringBuilder strData = new StringBuilder(256);
			if (m_IndexName == null) str = String.Format("{0} ", m_IndexNumber);
			else str = m_IndexName + " ";

			switch (m_TypeData)
			{
				case _TypeData.STRING:
					str += String.Format("CHAR {0}:{1}", m_DataString.Length, m_DataString);
					break;
				case _TypeData.INT:
					str += String.Format("INT {0}:", m_DataInt.Length);
					foreach (int val in m_DataInt) strData.AppendFormat("{0} ", val);
					str += strData;
					break;
				case _TypeData.FLOAT:
					str += String.Format("FLOAT {0}:", m_DataFloat.Length);
					foreach (float val in m_DataFloat) strData.AppendFormat("{0:F5} ", val);
					str += strData;
					break;
				case _TypeData.BOOL:
					str += String.Format("BOOL {0}:", m_DataBool.Length);
					foreach (bool val in m_DataBool) strData.AppendFormat("{0} ", val);
					str += strData;
					break;
			}
			return str;
		}

		public void RetrieveStringData(String data)
		{
			m_TypeData = _TypeData.STRING;
			m_DataString = data;
		}
		public void RetrieveIntData(int cnt, String data)
		{
			m_TypeData = _TypeData.INT;
			String[] cells = data.Split(new Char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
			if (cnt > cells.Length) cnt = cells.Length;
			m_DataInt = new int[cnt];
			for (int c = 0; c < cnt; c++)
			{
				if (!int.TryParse(cells[c], out m_DataInt[c])) m_DataInt[c] = 0;
			}
		}
		public void RetrieveFloatData(int cnt, String data)
		{
			m_TypeData = _TypeData.FLOAT;
			String[] cells = data.Split(new Char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
			if (cnt > cells.Length) cnt = cells.Length;
			m_DataFloat = new float[cnt];
			for (int c = 0; c < cnt; c++)
			{
				if (!float.TryParse(cells[c], out m_DataFloat[c])) m_DataFloat[c] = 0.0f;
			}
		}
		public void RetrieveBoolData(int cnt, String data)
		{
			m_TypeData = _TypeData.BOOL;
			String[] cells = data.Split(new Char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
			if (cnt > cells.Length) cnt = cells.Length;
			m_DataBool = new bool[cnt];
			for (int c = 0; c < cnt; c++)
			{
				if (!bool.TryParse(cells[c], out m_DataBool[c])) m_DataBool[c] = false;
			}
		}
    }

	public class CSPlainData3
	{
		private CSPlainData_Node m_NodeTop, m_NodeNow;

		public CSPlainData3()
		{
			m_NodeTop = new CSPlainData_Node();
			m_NodeNow = m_NodeTop.BearNode("");
		}

		public void SetNameSpace(string name)
		{
			m_NodeNow = m_NodeTop.BearNode(name);
		}
		public void Set(int idx, string name, string data)
		{
			CSPlainData_Node node = m_NodeNow.BearNode(idx);
			CSPlainData_Leaf leaf = node.BearLeaf(name);
			leaf.SetData(data);
		}
		public void Set(int idx, string name, int[] data)
		{
			CSPlainData_Node node = m_NodeNow.BearNode(idx);
			CSPlainData_Leaf leaf = node.BearLeaf(name);
			leaf.SetData(data);
		}
		public void Set(int idx, string name, float[] data)
		{
			CSPlainData_Node node = m_NodeNow.BearNode(idx);
			CSPlainData_Leaf leaf = node.BearLeaf(name);
			leaf.SetData(data);
		}
		public void Set(int idx, string name, bool[] data)
		{
			CSPlainData_Node node = m_NodeNow.BearNode(idx);
			CSPlainData_Leaf leaf = node.BearLeaf(name);
			leaf.SetData(data);
		}
		public void Set(int idx, string name, int data)
		{
			CSPlainData_Node node = m_NodeNow.BearNode(idx);
			CSPlainData_Leaf leaf = node.BearLeaf(name);
			leaf.SetData(new int[] { data });
		}
		public void Set(int idx, string name, float data)
		{
			CSPlainData_Node node = m_NodeNow.BearNode(idx);
			CSPlainData_Leaf leaf = node.BearLeaf(name);
			leaf.SetData(new float[] { data });
		}
		public void Set(int idx, string name, bool data)
		{
			CSPlainData_Node node = m_NodeNow.BearNode(idx);
			CSPlainData_Leaf leaf = node.BearLeaf(name);
			leaf.SetData(new bool[] { data });
		}

		public int Get(int idx, string name, out string data)
		{
			data = null;
			CSPlainData_Node node = m_NodeNow.SearchNode(idx);
			if (node == null) return 0;
			CSPlainData_Leaf leaf = (CSPlainData_Leaf)node.SearchNode(name);
			if (leaf == null) return 0;
			return leaf.GetData(out data);
		}
		public int Get(int idx, string name, out int[] data)
		{
			data = null;
			CSPlainData_Node node = m_NodeNow.SearchNode(idx);
			if (node == null) return 0;
			CSPlainData_Leaf leaf = (CSPlainData_Leaf)node.SearchNode(name);
			if (leaf == null) return 0;
			return leaf.GetData(out data);
		}
		public int Get(int idx, string name, out float[] data)
		{
			data = null;
			CSPlainData_Node node = m_NodeNow.SearchNode(idx);
			if (node == null) return 0;
			CSPlainData_Leaf leaf = (CSPlainData_Leaf)node.SearchNode(name);
			if (leaf == null) return 0;
			return leaf.GetData(out data);
		}
		public int Get(int idx, string name, out bool[] data)
		{
			data = null;
			CSPlainData_Node node = m_NodeNow.SearchNode(idx);
			if (node == null) return 0;
			CSPlainData_Leaf leaf = (CSPlainData_Leaf)node.SearchNode(name);
			if (leaf == null) return 0;
			return leaf.GetData(out data);
		}
		public int Get(int idx, string name, out int data)
		{
			data = 0;
			CSPlainData_Node node = m_NodeNow.SearchNode(idx);
			if (node == null) return 0;
			CSPlainData_Leaf leaf = (CSPlainData_Leaf)node.SearchNode(name);
			if (leaf == null) return 0;

			int[] tmp;
			if (leaf.GetData(out tmp) == 0) return 0;
			data = tmp[0];
			return 1;
		}
		public int Get(int idx, string name, out float data)
		{
			data = 0.0f;
			CSPlainData_Node node = m_NodeNow.SearchNode(idx);
			if (node == null) return 0;
			CSPlainData_Leaf leaf = (CSPlainData_Leaf)node.SearchNode(name);
			if (leaf == null) return 0;

			float[] tmp;
			if (leaf.GetData(out tmp) == 0) return 0;
			data = tmp[0];
			return 1;
		}
		public int Get(int idx, string name, out bool data)
		{
			data = false;
			CSPlainData_Node node = m_NodeNow.SearchNode(idx);
			if (node == null) return 0;
			CSPlainData_Leaf leaf = (CSPlainData_Leaf)node.SearchNode(name);
			if (leaf == null) return 0;

			bool[] tmp;
			if (leaf.GetData(out tmp) == 0) return 0;
			data = tmp[0];
			return 1;
		}

		public bool Save(String loc)
		{
			StreamWriter writer = new StreamWriter(loc, false, Encoding.UTF8);
			if (writer == null) return false;

			CSPlainData_Node space = m_NodeTop.m_NodeChildTop;
			while (space != null)
			{
				if (space.m_IndexName != "")
					writer.WriteLine("namespace {0} {{", space.m_IndexName);

				CSPlainData_Node node = space.m_NodeChildTop;
				while (node != null)
				{
					CSPlainData_Leaf leaf = (CSPlainData_Leaf)node.m_NodeChildTop;
					while (leaf != null)
					{
						writer.WriteLine("{0} {1}", node.m_IndexNumber, leaf.PrintLine());
						leaf = (CSPlainData_Leaf)leaf.m_NodeNext;
					}

					node = node.m_NodeNext;
				}

				if (space.m_IndexName != "") writer.WriteLine("}");
				space = space.m_NodeNext;
			}
			writer.Close();
			return true;
		}

		public bool Load(String loc)
		{
			StreamReader reader = new StreamReader(loc, Encoding.UTF8);
			if (reader == null) return false;

			Char[] sep = new Char[] { ' ' };
			string line;
			int stair = 0;
			Regex regSpace = new Regex(@"namespace[ ]+([\S]+)\s", RegexOptions.IgnoreCase);

			while ((line = reader.ReadLine()) != null)
			{
				int sharp = line.IndexOf('#');
				if (sharp == 0) continue;
				if (sharp > 0) line = line.Substring(0, sharp - 1);

				if (stair <= 0)
				{
					Match m = regSpace.Match(line);
					if (m.Groups.Count > 1)
					{
						SetNameSpace(m.Groups[1].Value);
						if (line.Contains("{")) stair++;
						if (line.Contains("}")) stair--;
						continue;
					}
				}
				if (line.Contains("{")) stair++;
				if (line.Contains("}")) stair--;

				int colon = line.IndexOf(':');
				if (colon < 0) continue;
				string strIndex, data;
				strIndex = line.Substring(0, colon);
				data = line.Substring(colon + 1);

				string[] indexCell = strIndex.Split(sep, StringSplitOptions.RemoveEmptyEntries);
				if( indexCell.Length < 3 ) continue;

				int number;
				if (!int.TryParse(indexCell[0], out number)) continue;
				string name = indexCell[1];
				int cntData = 0;
				if (indexCell.Length == 4) int.TryParse(indexCell[3], out cntData);

				///// 新しいノード
				CSPlainData_Node node = m_NodeNow.BearNode(number);
				CSPlainData_Leaf leaf = node.BearLeaf(name);

				switch (indexCell[2])
				{
					case "CHAR":
					case "C":
						leaf.RetrieveStringData(data);
						break;
					case "INT":
					case "I":
						if (cntData > 0) leaf.RetrieveIntData(cntData, data);
						break;
					case "FLOAT":
					case "F":
						if (cntData > 0) leaf.RetrieveFloatData(cntData, data);
						break;
					case "BOOL":
					case "B":
						if (cntData > 0) leaf.RetrieveBoolData(cntData, data);
						break;
				}
			}
			reader.Close();

			MakeCache();
			return true;
		}

		public void MakeCache()
		{
			CSPlainData_Node node = m_NodeTop.m_NodeChildTop;
			while (node != null)
			{
				node.MakeCacheNumber();
				node = node.m_NodeNext;
			}
		}
	}
}
